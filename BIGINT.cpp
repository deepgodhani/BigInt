#include <bits/stdc++.h>

using namespace std;

class bigInt {
    vector<int> num;
    bool isNeg;
    int base = 10;

public:
    bigInt() : isNeg(false) {}

    bigInt(vector<int> v) : isNeg(false), num(v) {
        trimLeadingZeros();
    }

    bigInt(string s) {
        int j = 0;
        isNeg = false;
        if (s[j] == '-') {
            isNeg = true;
            j++;
        }
        for (int i = s.length() - 1; i >= j; i--) num.push_back(s[i] - '0');
        trimLeadingZeros();
    }

    bigInt(long long n) {
        isNeg = false;
        if (n < 0) {
            isNeg = true;
            n = -n;
        }
        while (n) {
            num.push_back(n % base);
            n /= base;
        }
        if (num.empty()) num.push_back(0); // Handle zero case
    }

    // Trim leading zeros from the number
    void trimLeadingZeros() {
        while (num.size() > 1 && num.back() == 0) num.pop_back();
    }

    bigInt operator+(const bigInt& other) const {
        bigInt ans;
        if (isNeg && other.isNeg) {
            ans = addAbsValues(*this, other);
            ans.isNeg = true;
            return ans;
        }
        if (!isNeg && !other.isNeg) {
            return addAbsValues(*this, other);
        }
        ans = subAbsValues(*this, other);
        if (*this > other) ans.isNeg = isNeg;
        else ans.isNeg = other.isNeg;
        ans.trimLeadingZeros();
        return ans;
    }

    bigInt operator-(const bigInt& other) const {
        bigInt result;
        if (!isNeg && !other.isNeg) return subAbsValues(*this, other);
        if (isNeg && !other.isNeg) {
            result = addAbsValues(*this, other);
            result.isNeg = true;
            return result;
        }
        if (!isNeg && other.isNeg) {
            return addAbsValues(*this, other);
        }
        if (isNeg && other.isNeg) {
            return subAbsValues(other, *this);
        }
        result.trimLeadingZeros();
        return result;
    }

    bigInt operator*(const bigInt& other) const {
        bigInt a1 = *this;
        bigInt a2 = other;

        int maxSize = max(a1.num.size(), a2.num.size());
        if (a1.num.size() < maxSize) a1 = addPaddingZeros(a1.num, maxSize);
        if (a2.num.size() < maxSize) a2 = addPaddingZeros(a2.num, maxSize);

        bigInt result = karatsuba(a1, a2);
        result.trimLeadingZeros();
        return result;
    }

    bool operator>(const bigInt& other) const {
        if (isNeg != other.isNeg) return other.isNeg;
        if (isNeg) return isLesser(*this, other);
        return isGreater(*this, other);
    }

    bool operator<(const bigInt& other) const {
        if (isNeg != other.isNeg) return isNeg;
        if (isNeg) return isGreater(*this, other);
        return isLesser(*this, other);
    }

    bigInt addPaddingZeros(const vector<int>& a, int s) const {
        vector<int> result = a;
        result.insert(result.end(), s - result.size(), 0);
        return bigInt(result);
    }

    static bigInt shift(const bigInt& n, int m) {
        vector<int> result = n.num;
        result.insert(result.begin(), m, 0);
        return bigInt(result);
    }

    static pair<bigInt, bigInt> split(const bigInt& n, int m2) {
        if (m2 >= n.num.size()) {
            return {bigInt({0}), n};
        }
        vector<int> high(n.num.begin() + m2, n.num.end());
        vector<int> low(n.num.begin(), n.num.begin() + m2);
        return {bigInt(high), bigInt(low)};
    }

    bigInt karatsuba(const bigInt& num1, const bigInt& num2) const {
        // Base case: Use long multiplication for small numbers
        if (num1.num.size() <= 1 || num2.num.size() <= 1) {
            return longMultiplication(num1, num2);
        }

        int m = max(num1.num.size(), num2.num.size());
        int m2 = m / 2;

        bigInt high1, low1, high2, low2;
        tie(high1, low1) = split(num1, m2);
        tie(high2, low2) = split(num2, m2);

        bigInt z0 = karatsuba(low1, low2);
        bigInt z1 = karatsuba(low1 + high1, low2 + high2);
        bigInt z2 = karatsuba(high1, high2);

        return shift(z2, 2 * m2) + shift(z1 - z2 - z0, m2) + z0;
    }

    bigInt longMultiplication(const bigInt& a, const bigInt& b) const {
        bigInt result;
        result.num.resize(a.num.size() + b.num.size(), 0);

        for (size_t i = 0; i < a.num.size(); i++) {
            int carry = 0;
            for (size_t j = 0; j < b.num.size() || carry; j++) {
                long long cur = result.num[i + j] + a.num[i] * (j < b.num.size() ? b.num[j] : 0) + carry;
                result.num[i + j] = cur % base;
                carry = cur / base;
            }
        }

        result.trimLeadingZeros();
        return result;
    }

    bigInt subAbsValues(const bigInt& a, const bigInt& b) const {
        bigInt result;
        if (b > a) {
            result = subAbsValues(b, a);
            result.isNeg = true;
            return result;
        }

        int borrow = 0;
        int i = 0;
        int len = b.num.size();

        for (i = 0; i < len; i++) {
            int s = a.num[i] - b.num[i] - borrow;
            if (s < 0) {
                s += base;
                borrow = 1;
            } else {
                borrow = 0;
            }
            result.num.push_back(s);
        }

        while (i < a.num.size()) {
            int s = a.num[i] - borrow;
            if (s < 0) {
                s += base;
                borrow = 1;
            } else {
                borrow = 0;
            }
            result.num.push_back(s);
            i++;
        }

        result.trimLeadingZeros();
        return result;
    }

    bigInt addAbsValues(const bigInt& a, const bigInt& b) const {
        bigInt result;
        int len = min(a.num.size(), b.num.size());

        int carry = 0;
        int i = 0;
        for (i = 0; i < len; i++) {
            int s = carry + a.num[i] + b.num[i];
            result.num.push_back(s % base);
            carry = s / base;
        }

        while (i < b.num.size()) {
            int s = b.num[i] + carry;
            result.num.push_back(s % base);
            carry = s / base;
            i++;
        }

        while (i < a.num.size()) {
            int s = carry + a.num[i];
            result.num.push_back(s % base);
            carry = s / base;
            i++;
        }

        while (carry) {
            result.num.push_back(carry % base);
            carry /= base;
        }

        result.trimLeadingZeros();
        return result;
    }

    bool isGreater(const bigInt& a, const bigInt& b) const {
        if (a.num.size() != b.num.size()) return a.num.size() > b.num.size();
        for (int i = a.num.size() - 1; i >= 0; i--) {
            if (a.num[i] != b.num[i]) return a.num[i] > b.num[i];
        }
        return false;
    }

    bool isLesser(const bigInt& a, const bigInt& b) const {
        if (a.num.size() != b.num.size()) return a.num.size() < b.num.size();
        for (int i = a.num.size() - 1; i >= 0; i--) {
            if (a.num[i] != b.num[i]) return a.num[i] < b.num[i];
        }
        return false;
    }

    friend ostream& operator<<(ostream& os, const bigInt& b) {
        if (b.isNeg) os << '-';
        for (int i = b.num.size() - 1; i >= 0; i--) os << b.num[i];
        return os;
    }
};
	//both mat has same dimension n x n
	bigInt multMat(vector<vector<bigInt> >& a,vector<vector<bigInt> >& b){
		int n=a.size();

		for(int i=0;i<n;i++){
			for(int i=0;i<n;i++){
				
			}
		}
	}

int main() {
    bigInt a1("120");
    bigInt a2("987654321098765432109876543210");
    bigInt a3("2");
    bigInt c1 = a2 - a1;

    cout << c1 << endl; 
}