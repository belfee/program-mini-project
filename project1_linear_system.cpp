// ================================================================
// Project 1 - Linear System
// All-in-one single file (combined .h + .cpp)
// ================================================================

#include <iostream>
#include <cassert>
#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include <algorithm>
#include <ctime>

// -------- Vector.h --------

class Vector {
private:
    int mSize;
    double* mData;

public:
    // Constructors & Destructor
    Vector(int size);
    Vector(const Vector& other);       // Copy constructor
    ~Vector();

    // Getters
    int GetSize() const { return mSize; }

    // Operators
    Vector& operator=(const Vector& other);
    Vector  operator-() const;                      // Unary minus
    Vector  operator+(const Vector& other) const;
    Vector  operator-(const Vector& other) const;
    Vector  operator*(double scalar) const;
    friend Vector operator*(double scalar, const Vector& v);

    // Element access: 0-based [] with bounds check, 1-based () per spec
    double& operator[](int index);
    double  operator[](int index) const;
    double& operator()(int index);                  // 1-based
    double  operator()(int index) const;

    // Dot product
    double Dot(const Vector& other) const;

    // Norm
    double Norm() const;

    // Print
    void Print(const std::string& name = "") const;
};


// -------- Vector.cpp --------

// ─── Constructors / Destructor ────────────────────────────────────────────────

Vector::Vector(int size) : mSize(size) {
    assert(size > 0);
    mData = new double[mSize]();   // zero-initialise
}

Vector::Vector(const Vector& other) : mSize(other.mSize) {
    mData = new double[mSize];
    for (int i = 0; i < mSize; ++i)
        mData[i] = other.mData[i];
}

Vector::~Vector() {
    delete[] mData;
}

// ─── Assignment ───────────────────────────────────────────────────────────────

Vector& Vector::operator=(const Vector& other) {
    if (this == &other) return *this;
    assert(mSize == other.mSize);
    for (int i = 0; i < mSize; ++i)
        mData[i] = other.mData[i];
    return *this;
}

// ─── Unary minus ─────────────────────────────────────────────────────────────

Vector Vector::operator-() const {
    Vector result(mSize);
    for (int i = 0; i < mSize; ++i)
        result.mData[i] = -mData[i];
    return result;
}

// ─── Binary operators ────────────────────────────────────────────────────────

Vector Vector::operator+(const Vector& other) const {
    assert(mSize == other.mSize);
    Vector result(mSize);
    for (int i = 0; i < mSize; ++i)
        result.mData[i] = mData[i] + other.mData[i];
    return result;
}

Vector Vector::operator-(const Vector& other) const {
    assert(mSize == other.mSize);
    Vector result(mSize);
    for (int i = 0; i < mSize; ++i)
        result.mData[i] = mData[i] - other.mData[i];
    return result;
}

Vector Vector::operator*(double scalar) const {
    Vector result(mSize);
    for (int i = 0; i < mSize; ++i)
        result.mData[i] = mData[i] * scalar;
    return result;
}

Vector operator*(double scalar, const Vector& v) {
    return v * scalar;
}

// ─── Element access ───────────────────────────────────────────────────────────

// 0-based with bounds check
double& Vector::operator[](int index) {
    assert(index >= 0 && index < mSize);
    return mData[index];
}
double Vector::operator[](int index) const {
    assert(index >= 0 && index < mSize);
    return mData[index];
}

// 1-based
double& Vector::operator()(int index) {
    assert(index >= 1 && index <= mSize);
    return mData[index - 1];
}
double Vector::operator()(int index) const {
    assert(index >= 1 && index <= mSize);
    return mData[index - 1];
}

// ─── Utilities ────────────────────────────────────────────────────────────────

double Vector::Dot(const Vector& other) const {
    assert(mSize == other.mSize);
    double sum = 0.0;
    for (int i = 0; i < mSize; ++i)
        sum += mData[i] * other.mData[i];
    return sum;
}

double Vector::Norm() const {
    return std::sqrt(Dot(*this));
}

void Vector::Print(const std::string& name) const {
    if (!name.empty()) std::cout << name << " = ";
    std::cout << "[";
    for (int i = 0; i < mSize; ++i) {
        std::cout << mData[i];
        if (i < mSize - 1) std::cout << ", ";
    }
    std::cout << "]\n";
}


// -------- Matrix.h --------

class Matrix {
private:
    int     mNumRows;
    int     mNumCols;
    double** mData;

    void Allocate();
    void Deallocate();

public:
    // Constructors & Destructor
    Matrix(int numRows, int numCols);
    Matrix(const Matrix& other);        // Copy constructor
    ~Matrix();

    // Getters
    int GetNumRows() const { return mNumRows; }
    int GetNumCols() const { return mNumCols; }

    // Element access: 1-based ()
    double& operator()(int i, int j);
    double  operator()(int i, int j) const;

    // Operators
    Matrix& operator=(const Matrix& other);
    Matrix  operator-() const;
    Matrix  operator+(const Matrix& other) const;
    Matrix  operator-(const Matrix& other) const;
    Matrix  operator*(const Matrix& other) const;
    Matrix  operator*(double scalar) const;
    Vector  operator*(const Vector& v) const;
    friend Matrix operator*(double scalar, const Matrix& m);

    // Linear algebra methods
    double Determinant() const;
    Matrix Inverse() const;
    Matrix PseudoInverse() const;       // Moore-Penrose via SVD (power-iteration approach)
    Matrix Transpose() const;

    // Print
    void Print(const std::string& name = "") const;
};


// -------- Matrix.cpp --------

// ─── Private helpers ──────────────────────────────────────────────────────────

void Matrix::Allocate() {
    mData = new double*[mNumRows];
    for (int i = 0; i < mNumRows; ++i)
        mData[i] = new double[mNumCols]();
}

void Matrix::Deallocate() {
    for (int i = 0; i < mNumRows; ++i)
        delete[] mData[i];
    delete[] mData;
}

// ─── Constructors / Destructor ────────────────────────────────────────────────

Matrix::Matrix(int numRows, int numCols) : mNumRows(numRows), mNumCols(numCols) {
    assert(numRows > 0 && numCols > 0);
    Allocate();
}

Matrix::Matrix(const Matrix& other) : mNumRows(other.mNumRows), mNumCols(other.mNumCols) {
    Allocate();
    for (int i = 0; i < mNumRows; ++i)
        for (int j = 0; j < mNumCols; ++j)
            mData[i][j] = other.mData[i][j];
}

Matrix::~Matrix() {
    Deallocate();
}

// ─── Element access (1-based) ─────────────────────────────────────────────────

double& Matrix::operator()(int i, int j) {
    assert(i >= 1 && i <= mNumRows && j >= 1 && j <= mNumCols);
    return mData[i-1][j-1];
}
double Matrix::operator()(int i, int j) const {
    assert(i >= 1 && i <= mNumRows && j >= 1 && j <= mNumCols);
    return mData[i-1][j-1];
}

// ─── Assignment ───────────────────────────────────────────────────────────────

Matrix& Matrix::operator=(const Matrix& other) {
    if (this == &other) return *this;
    assert(mNumRows == other.mNumRows && mNumCols == other.mNumCols);
    for (int i = 0; i < mNumRows; ++i)
        for (int j = 0; j < mNumCols; ++j)
            mData[i][j] = other.mData[i][j];
    return *this;
}

// ─── Arithmetic operators ─────────────────────────────────────────────────────

Matrix Matrix::operator-() const {
    Matrix result(mNumRows, mNumCols);
    for (int i = 0; i < mNumRows; ++i)
        for (int j = 0; j < mNumCols; ++j)
            result.mData[i][j] = -mData[i][j];
    return result;
}

Matrix Matrix::operator+(const Matrix& other) const {
    assert(mNumRows == other.mNumRows && mNumCols == other.mNumCols);
    Matrix result(mNumRows, mNumCols);
    for (int i = 0; i < mNumRows; ++i)
        for (int j = 0; j < mNumCols; ++j)
            result.mData[i][j] = mData[i][j] + other.mData[i][j];
    return result;
}

Matrix Matrix::operator-(const Matrix& other) const {
    assert(mNumRows == other.mNumRows && mNumCols == other.mNumCols);
    Matrix result(mNumRows, mNumCols);
    for (int i = 0; i < mNumRows; ++i)
        for (int j = 0; j < mNumCols; ++j)
            result.mData[i][j] = mData[i][j] - other.mData[i][j];
    return result;
}

Matrix Matrix::operator*(const Matrix& other) const {
    assert(mNumCols == other.mNumRows);
    Matrix result(mNumRows, other.mNumCols);
    for (int i = 0; i < mNumRows; ++i)
        for (int j = 0; j < other.mNumCols; ++j)
            for (int k = 0; k < mNumCols; ++k)
                result.mData[i][j] += mData[i][k] * other.mData[k][j];
    return result;
}

Matrix Matrix::operator*(double scalar) const {
    Matrix result(mNumRows, mNumCols);
    for (int i = 0; i < mNumRows; ++i)
        for (int j = 0; j < mNumCols; ++j)
            result.mData[i][j] = mData[i][j] * scalar;
    return result;
}

Matrix operator*(double scalar, const Matrix& m) {
    return m * scalar;
}

Vector Matrix::operator*(const Vector& v) const {
    assert(mNumCols == v.GetSize());
    Vector result(mNumRows);
    for (int i = 1; i <= mNumRows; ++i) {
        double sum = 0.0;
        for (int j = 1; j <= mNumCols; ++j)
            sum += (*this)(i, j) * v(j);
        result(i) = sum;
    }
    return result;
}

// ─── Transpose ────────────────────────────────────────────────────────────────

Matrix Matrix::Transpose() const {
    Matrix result(mNumCols, mNumRows);
    for (int i = 1; i <= mNumRows; ++i)
        for (int j = 1; j <= mNumCols; ++j)
            result(j, i) = (*this)(i, j);
    return result;
}

// ─── Determinant (LU decomposition) ──────────────────────────────────────────

double Matrix::Determinant() const {
    assert(mNumRows == mNumCols);
    int n = mNumRows;
    Matrix LU(*this);
    double det = 1.0;
    int sign = 1;

    for (int col = 0; col < n; ++col) {
        // Partial pivot
        int pivotRow = col;
        double maxVal = std::abs(LU.mData[col][col]);
        for (int row = col + 1; row < n; ++row) {
            if (std::abs(LU.mData[row][col]) > maxVal) {
                maxVal = std::abs(LU.mData[row][col]);
                pivotRow = row;
            }
        }
        if (pivotRow != col) {
            std::swap(LU.mData[col], LU.mData[pivotRow]);
            sign *= -1;
        }
        if (std::abs(LU.mData[col][col]) < 1e-12) return 0.0;

        det *= LU.mData[col][col];
        for (int row = col + 1; row < n; ++row) {
            double factor = LU.mData[row][col] / LU.mData[col][col];
            for (int k = col; k < n; ++k)
                LU.mData[row][k] -= factor * LU.mData[col][k];
        }
    }
    return sign * det;
}

// ─── Inverse (Gauss-Jordan with partial pivoting) ────────────────────────────

Matrix Matrix::Inverse() const {
    assert(mNumRows == mNumCols);
    int n = mNumRows;

    // Build augmented matrix [A | I]
    Matrix aug(n, 2 * n);
    for (int i = 1; i <= n; ++i)
        for (int j = 1; j <= n; ++j)
            aug(i, j) = (*this)(i, j);
    for (int i = 1; i <= n; ++i)
        aug(i, n + i) = 1.0;

    // Forward elimination with partial pivot
    for (int col = 1; col <= n; ++col) {
        // Find pivot
        int pivotRow = col;
        double maxVal = std::abs(aug(col, col));
        for (int row = col + 1; row <= n; ++row) {
            if (std::abs(aug(row, col)) > maxVal) {
                maxVal = std::abs(aug(row, col));
                pivotRow = row;
            }
        }
        assert(maxVal > 1e-12 && "Matrix is singular");

        // Swap rows
        if (pivotRow != col)
            for (int j = 1; j <= 2 * n; ++j)
                std::swap(aug(col, j), aug(pivotRow, j));

        // Scale pivot row
        double pivot = aug(col, col);
        for (int j = 1; j <= 2 * n; ++j)
            aug(col, j) /= pivot;

        // Eliminate column
        for (int row = 1; row <= n; ++row) {
            if (row == col) continue;
            double factor = aug(row, col);
            for (int j = 1; j <= 2 * n; ++j)
                aug(row, j) -= factor * aug(col, j);
        }
    }

    // Extract right half
    Matrix inv(n, n);
    for (int i = 1; i <= n; ++i)
        for (int j = 1; j <= n; ++j)
            inv(i, j) = aug(i, n + j);
    return inv;
}

// ─── Moore-Penrose Pseudo-Inverse  A† = (AᵀA)⁻¹Aᵀ  (for full column rank) ──
// Falls back to symmetric case when A is square & invertible.

Matrix Matrix::PseudoInverse() const {
    Matrix At = Transpose();
    Matrix AtA = At * (*this);   // n×n

    // Check if AtA is invertible
    double det = AtA.Determinant();
    if (std::abs(det) > 1e-10) {
        // Full column rank: A† = (AᵀA)⁻¹Aᵀ
        return AtA.Inverse() * At;
    } else {
        // Full row rank: A† = Aᵀ(AAᵀ)⁻¹
        Matrix AAt = (*this) * At;
        return At * AAt.Inverse();
    }
}

// ─── Print ────────────────────────────────────────────────────────────────────

void Matrix::Print(const std::string& name) const {
    if (!name.empty()) std::cout << name << ":\n";
    for (int i = 0; i < mNumRows; ++i) {
        std::cout << "[ ";
        for (int j = 0; j < mNumCols; ++j)
            std::cout << std::setw(10) << std::setprecision(4) << mData[i][j] << " ";
        std::cout << "]\n";
    }
}


// -------- LinearSystem.h --------

// ─── LinearSystem ─────────────────────────────────────────────────────────────
// Solves  A x = b  by Gaussian elimination with partial pivoting.

class LinearSystem {
protected:
    int     mSize;
    Matrix* mpA;
    Vector* mpb;

private:
    // Prevent accidental copy (no copy constructor allowed)
    LinearSystem(const LinearSystem&) = delete;
    LinearSystem& operator=(const LinearSystem&) = delete;

    // No default constructor
    LinearSystem() = delete;

public:
    LinearSystem(const Matrix& A, const Vector& b);
    virtual ~LinearSystem();

    virtual Vector Solve() const;
};

// ─── PosSymLinSystem ──────────────────────────────────────────────────────────
// Solves positive-definite symmetric systems using the Conjugate Gradient method.

class PosSymLinSystem : public LinearSystem {
public:
    PosSymLinSystem(const Matrix& A, const Vector& b);
    ~PosSymLinSystem() override = default;

    Vector Solve() const override;   // Conjugate Gradient

private:
    bool IsSymmetric() const;
};


// -------- LinearSystem.cpp --------

// ═════════════════════════════════════════════════════════════════════════════
//  LinearSystem
// ═════════════════════════════════════════════════════════════════════════════

LinearSystem::LinearSystem(const Matrix& A, const Vector& b) {
    assert(A.GetNumRows() == A.GetNumCols());
    assert(A.GetNumRows() == b.GetSize());
    mSize = b.GetSize();
    mpA   = new Matrix(A);
    mpb   = new Vector(b);
}

LinearSystem::~LinearSystem() {
    delete mpA;
    delete mpb;
}

// ─── Gaussian Elimination with Partial Pivoting ───────────────────────────────

Vector LinearSystem::Solve() const {
    // Work on copies
    Matrix A(*mpA);
    Vector b(*mpb);
    int n = mSize;

    for (int col = 1; col <= n; ++col) {
        // --- Partial pivot ---
        int pivotRow = col;
        double maxVal = std::abs(A(col, col));
        for (int row = col + 1; row <= n; ++row) {
            if (std::abs(A(row, col)) > maxVal) {
                maxVal = std::abs(A(row, col));
                pivotRow = row;
            }
        }
        assert(maxVal > 1e-12 && "Singular or near-singular matrix in LinearSystem::Solve");

        // Swap rows
        if (pivotRow != col) {
            for (int j = 1; j <= n; ++j)
                std::swap(A(col, j), A(pivotRow, j));
            std::swap(b(col), b(pivotRow));
        }

        // --- Eliminate below ---
        for (int row = col + 1; row <= n; ++row) {
            double factor = A(row, col) / A(col, col);
            for (int j = col; j <= n; ++j)
                A(row, j) -= factor * A(col, j);
            b(row) -= factor * b(col);
        }
    }

    // --- Back substitution ---
    Vector x(n);
    for (int i = n; i >= 1; --i) {
        double sum = b(i);
        for (int j = i + 1; j <= n; ++j)
            sum -= A(i, j) * x(j);
        x(i) = sum / A(i, i);
    }
    return x;
}

// ═════════════════════════════════════════════════════════════════════════════
//  PosSymLinSystem  — Conjugate Gradient
// ═════════════════════════════════════════════════════════════════════════════

PosSymLinSystem::PosSymLinSystem(const Matrix& A, const Vector& b)
    : LinearSystem(A, b) {
    assert(IsSymmetric() && "PosSymLinSystem: matrix must be symmetric");
}

bool PosSymLinSystem::IsSymmetric() const {
    const double tol = 1e-9;
    for (int i = 1; i <= mSize; ++i)
        for (int j = i + 1; j <= mSize; ++j)
            if (std::abs((*mpA)(i, j) - (*mpA)(j, i)) > tol)
                return false;
    return true;
}

Vector PosSymLinSystem::Solve() const {
    const Matrix& A = *mpA;
    const Vector& b = *mpb;
    int n = mSize;

    Vector x(n);            // initial guess = zero
    Vector r(b);            // r = b - A*x  =  b  (x=0)
    Vector p(r);            // search direction
    double rDotR = r.Dot(r);

    const double tol     = 1e-10;
    const int    maxIter = 10 * n;

    for (int iter = 0; iter < maxIter; ++iter) {
        if (std::sqrt(rDotR) < tol) break;

        Vector Ap    = A * p;
        double alpha = rDotR / p.Dot(Ap);   // step size

        // x = x + alpha * p
        for (int i = 1; i <= n; ++i)
            x(i) += alpha * p(i);

        // r = r - alpha * Ap
        for (int i = 1; i <= n; ++i)
            r(i) -= alpha * Ap(i);

        double rDotRNew = r.Dot(r);
        double beta     = rDotRNew / rDotR;

        // p = r + beta * p
        for (int i = 1; i <= n; ++i)
            p(i) = r(i) + beta * p(i);

        rDotR = rDotRNew;
    }
    return x;
}


// -------- LinearRegression.h --------

// ─── LinearRegression ─────────────────────────────────────────────────────────
// Solves  PRP = x1*MYCT + x2*MMIN + x3*MMAX + x4*CACH + x5*CHMIN + x6*CHMAX
// using the Moore-Penrose pseudo-inverse: x = (AᵀA)⁻¹Aᵀb

class LinearRegression {
private:
    // Feature indices (0-based in raw data)
    // Col: 0=vendor,1=model,2=MYCT,3=MMIN,4=MMAX,5=CACH,6=CHMIN,7=CHMAX,8=PRP,9=ERP
    static const int NUM_FEATURES = 6;
    static const int FEATURE_START = 2;   // MYCT at index 2
    static const int TARGET_IDX    = 8;   // PRP

    Vector mParams;     // learned parameters (size 6)
    bool   mTrained;

    struct Record {
        double features[NUM_FEATURES];
        double target;
    };

    std::vector<Record> LoadCSV(const std::string& filepath) const;
    double RMSE(const Matrix& X, const Vector& y) const;

public:
    LinearRegression();

    // Train on file, split 80/20, return train/test RMSE
    void Train(const std::string& filepath,
               double trainRatio = 0.8,
               bool   verbose    = true);

    // Predict for a single feature vector [MYCT,MMIN,MMAX,CACH,CHMIN,CHMAX]
    double Predict(const double features[NUM_FEATURES]) const;

    void PrintParams() const;
};


// -------- LinearRegression.cpp --------

// ─── Constructor ──────────────────────────────────────────────────────────────

LinearRegression::LinearRegression() : mParams(NUM_FEATURES), mTrained(false) {}

// ─── CSV Loader ───────────────────────────────────────────────────────────────
// Expected format: comma-separated, 10 columns per row.
// Rows with missing numeric fields are skipped.

std::vector<LinearRegression::Record>
LinearRegression::LoadCSV(const std::string& filepath) const {
    std::ifstream file(filepath);
    if (!file.is_open())
        throw std::runtime_error("Cannot open file: " + filepath);

    std::vector<Record> records;
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::vector<std::string> tokens;
        std::stringstream ss(line);
        std::string token;
        while (std::getline(ss, token, ',')) {
            // trim whitespace
            size_t s = token.find_first_not_of(" \t\r\n");
            size_t e = token.find_last_not_of(" \t\r\n");
            tokens.push_back((s == std::string::npos) ? "" : token.substr(s, e - s + 1));
        }

        if ((int)tokens.size() < TARGET_IDX + 1) continue;

        Record rec;
        bool valid = true;
        for (int f = 0; f < NUM_FEATURES; ++f) {
            try {
                rec.features[f] = std::stod(tokens[FEATURE_START + f]);
            } catch (...) {
                valid = false; break;
            }
        }
        if (!valid) continue;

        try {
            rec.target = std::stod(tokens[TARGET_IDX]);
        } catch (...) {
            continue;
        }
        records.push_back(rec);
    }
    return records;
}

// ─── RMSE helper ──────────────────────────────────────────────────────────────

double LinearRegression::RMSE(const Matrix& X, const Vector& y) const {
    assert(mTrained);
    int n = y.GetSize();
    double sumSq = 0.0;
    for (int i = 1; i <= n; ++i) {
        double pred = 0.0;
        for (int j = 1; j <= NUM_FEATURES; ++j)
            pred += X(i, j) * mParams(j);
        double err = pred - y(i);
        sumSq += err * err;
    }
    return std::sqrt(sumSq / n);
}

// ─── Train ────────────────────────────────────────────────────────────────────

void LinearRegression::Train(const std::string& filepath,
                              double trainRatio,
                              bool   verbose) {
    auto records = LoadCSV(filepath);
    int total     = (int)records.size();
    int trainSize = (int)(total * trainRatio);
    int testSize  = total - trainSize;

    if (verbose)
        std::cout << "Dataset: " << total << " records  "
                  << "(train=" << trainSize << ", test=" << testSize << ")\n";

    // Build training matrices
    Matrix Xtrain(trainSize, NUM_FEATURES);
    Vector ytrain(trainSize);
    for (int i = 0; i < trainSize; ++i) {
        for (int j = 0; j < NUM_FEATURES; ++j)
            Xtrain(i+1, j+1) = records[i].features[j];
        ytrain(i+1) = records[i].target;
    }

    // Build test matrices
    Matrix Xtest(testSize, NUM_FEATURES);
    Vector ytest(testSize);
    for (int i = 0; i < testSize; ++i) {
        for (int j = 0; j < NUM_FEATURES; ++j)
            Xtest(i+1, j+1) = records[trainSize + i].features[j];
        ytest(i+1) = records[trainSize + i].target;
    }

    // Normal equations: x = (XᵀX)⁻¹ Xᵀ y  =  PseudoInverse(X) * y
    Matrix Xt   = Xtrain.Transpose();
    Matrix XtX  = Xt * Xtrain;
    Vector Xty  (NUM_FEATURES);

    // Xᵀy
    for (int j = 1; j <= NUM_FEATURES; ++j) {
        double s = 0.0;
        for (int i = 1; i <= trainSize; ++i)
            s += Xtrain(i, j) * ytrain(i);
        Xty(j) = s;
    }

    // Solve normal equation directly
    Matrix XtX_inv = XtX.Inverse();
    mParams        = XtX_inv * Xty;
    mTrained       = true;

    if (verbose) {
        PrintParams();
        double trainRMSE = RMSE(Xtrain, ytrain);
        double testRMSE  = RMSE(Xtest,  ytest);
        std::cout << std::fixed << std::setprecision(4);
        std::cout << "Train RMSE : " << trainRMSE << "\n";
        std::cout << "Test  RMSE : " << testRMSE  << "\n";
    }
}

// ─── Predict ──────────────────────────────────────────────────────────────────

double LinearRegression::Predict(const double features[NUM_FEATURES]) const {
    assert(mTrained && "Model not trained yet");
    double result = 0.0;
    for (int j = 0; j < NUM_FEATURES; ++j)
        result += features[j] * mParams(j+1);
    return result;
}

// ─── Print parameters ────────────────────────────────────────────────────────

void LinearRegression::PrintParams() const {
    const char* names[] = {"MYCT","MMIN","MMAX","CACH","CHMIN","CHMAX"};
    std::cout << "\nModel parameters:\n";
    for (int i = 0; i < NUM_FEATURES; ++i)
        std::cout << "  x" << (i+1) << " (" << names[i] << ") = "
                  << std::setw(12) << std::setprecision(6) << mParams(i+1) << "\n";
    std::cout << "\n";
}


// -------- main.cpp --------

// ─── Helper: print separator ─────────────────────────────────────────────────
static void section(const std::string& title) {
    std::cout << "\n══════════════════════════════════════════\n";
    std::cout << "  " << title << "\n";
    std::cout << "══════════════════════════════════════════\n";
}

// ─── Test Vector class ────────────────────────────────────────────────────────
void testVector() {
    section("TEST: Vector class");

    Vector u(3), v(3);
    u(1)=1; u(2)=2; u(3)=3;
    v(1)=4; v(2)=5; v(3)=6;

    u.Print("u");
    v.Print("v");
    (u + v).Print("u+v");
    (u - v).Print("u-v");
    (2.0 * u).Print("2*u");
    (-u).Print("-u");
    std::cout << "Dot(u,v) = " << u.Dot(v) << "  (expected 32)\n";
    std::cout << "Norm(u)  = " << u.Norm() << "  (expected 3.7417)\n";
}

// ─── Test Matrix class ────────────────────────────────────────────────────────
void testMatrix() {
    section("TEST: Matrix determinant, inverse, pseudo-inverse");

    // 3×3 test matrix
    Matrix A(3, 3);
    A(1,1)=2; A(1,2)=1; A(1,3)=-1;
    A(2,1)=-3;A(2,2)=-1;A(2,3)=2;
    A(3,1)=-2;A(3,2)=1; A(3,3)=2;

    A.Print("A");
    std::cout << "det(A) = " << A.Determinant() << "  (expected -1)\n";

    Matrix Ainv = A.Inverse();
    Ainv.Print("A^-1");

    Matrix I = A * Ainv;
    I.Print("A * A^-1 (should be Identity)");

    // Non-square pseudo-inverse: 4×3
    section("TEST: Moore-Penrose Pseudo-Inverse (4x3 matrix)");
    Matrix B(4, 3);
    B(1,1)=1; B(1,2)=2; B(1,3)=3;
    B(2,1)=4; B(2,2)=5; B(2,3)=6;
    B(3,1)=7; B(3,2)=8; B(3,3)=9;
    B(4,1)=1; B(4,2)=0; B(4,3)=1;

    B.Print("B (4x3)");
    Matrix Bpi = B.PseudoInverse();
    Bpi.Print("B† (3x4)");

    // Check: B† * B ≈ I (3×3)
    Matrix BpB = Bpi * B;
    BpB.Print("B† * B (should be ~I3)");
}

// ─── Test LinearSystem (Gaussian elimination) ────────────────────────────────
void testLinearSystem() {
    section("TEST: LinearSystem — Gaussian Elimination");

    // 3×3 example: solution x=[2, 3, -1]
    Matrix A(3,3);
    A(1,1)=2; A(1,2)=1; A(1,3)=-1;
    A(2,1)=-3;A(2,2)=-1;A(2,3)=2;
    A(3,1)=-2;A(3,2)=1; A(3,3)=2;

    Vector b(3);
    b(1)=8; b(2)=-11; b(3)=-3;

    LinearSystem sys(A, b);
    Vector x = sys.Solve();
    x.Print("x (expected [2, 3, -1])");
}

// ─── Test PosSymLinSystem (Conjugate Gradient) ───────────────────────────────
void testPosSymLinSystem() {
    section("TEST: PosSymLinSystem — Conjugate Gradient");

    // SPD matrix: A = [[4,1],[1,3]], b=[1,2] => x=[1/11, 7/11]
    Matrix A(2,2);
    A(1,1)=4; A(1,2)=1;
    A(2,1)=1; A(2,2)=3;

    Vector b(2);
    b(1)=1; b(2)=2;

    PosSymLinSystem sys(A, b);
    Vector x = sys.Solve();
    x.Print("x (expected [0.09091, 0.63636])");
}

// ─── Test under/over-determined via pseudo-inverse ───────────────────────────
void testPseudoSolve() {
    section("TEST: Over-determined system (least squares via pseudo-inverse)");

    // Over-determined: 4 equations, 2 unknowns
    //  x1 + x2 = 1
    // -x1 + x2 = 2
    // x2       = 3
    // x1       = 1
    // Least-squares solution: use A† b

    Matrix A(4, 2);
    A(1,1)=1;  A(1,2)=1;
    A(2,1)=-1; A(2,2)=1;
    A(3,1)=0;  A(3,2)=1;
    A(4,1)=1;  A(4,2)=0;

    Vector b(4);
    b(1)=1; b(2)=2; b(3)=3; b(4)=1;

    A.Print("A (4x2)");
    b.Print("b");

    Matrix Adag = A.PseudoInverse();
    Vector x    = Adag * b;
    x.Print("x (least-squares solution)");

    // Residual
    Vector res = A * x - b;
    std::cout << "Residual norm = " << res.Norm() << "\n";
}

// ─── Part B: Linear Regression on CPU dataset ────────────────────────────────
void testLinearRegression(const std::string& dataFile) {
    section("PART B: Linear Regression — CPU Performance");
    LinearRegression lr;
    try {
        lr.Train(dataFile, 0.8, true);

        // Sample prediction
        double sample[] = {125, 256, 6000, 256, 16, 128};
        std::cout << "Sample prediction [MYCT=125,MMIN=256,MMAX=6000,"
                     "CACH=256,CHMIN=16,CHMAX=128]: "
                  << lr.Predict(sample) << "\n";
    } catch (const std::exception& e) {
        std::cout << "[Skipped] " << e.what() << "\n";
        std::cout << "Place 'machine.data' in the data/ directory to enable Part B.\n";
    }
}

// ─── main ─────────────────────────────────────────────────────────────────────
int main() {
    testVector();
    testMatrix();
    testLinearSystem();
    testPosSymLinSystem();
    testPseudoSolve();
    testLinearRegression("data/machine.data");

    std::cout << "\nAll tests completed.\n";
    return 0;
}
