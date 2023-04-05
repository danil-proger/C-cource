#include <iostream>
#include <string>
#include <vector>

using namespace std;

class BigInteger {

    friend bool operator==(const BigInteger &first, const BigInteger &second);

    friend bool operator<(const BigInteger &first, const BigInteger &second);

    friend istream &operator>>(istream &input, BigInteger& data);

    friend ostream &operator<<(ostream &out, const BigInteger& data);

private:
    vector<int> bigint;
    static const int BASE = 1000000000;
    int sign;

public:
    BigInteger(int value);

    BigInteger();

    void zeros();

    void Sign();

    void shift();

    explicit operator bool() const;

    BigInteger& operator=(const BigInteger &other);

    BigInteger operator-() const;

    string toString() const;

    BigInteger &operator++();

    BigInteger &operator--();

    BigInteger operator++(int);

    BigInteger operator--(int);

    BigInteger &operator+=(const BigInteger &value);

    BigInteger &operator*=(const BigInteger &value);

    BigInteger &operator-=(const BigInteger &value);

    BigInteger &operator/=(const BigInteger &value);

    BigInteger &operator%=(const BigInteger &value);

};


bool operator==(const BigInteger &first, const BigInteger &second)  {
    if (first.sign != second.sign) return false;
    if (first.bigint.size() != second.bigint.size()) return false;
    int size = first.bigint.size();
    for (int i = 0; i < size; ++i){
        if (first.bigint[i] != second.bigint[i]) return false;
    }
    return true;
}

istream &operator>>(istream &input, BigInteger& data) {
    data.bigint.clear();
    string str;
    input >> str;
    if (not (str.empty())) {
        if (str[0] == '-') data.sign = -1, str = str.substr(1, str.length() - 1);
        else data.sign = 1;
        for (long long i = str.length(); i > 0; i -= 9) {
            if (i < 9) data.bigint.push_back(stoi(str.substr(0, i)));
            else data.bigint.push_back(stoi(str.substr(i - 9, 9)));
        }
    }
    data.zeros();
    return input;
}

void BigInteger::zeros() {
    while ((bigint.back() == 0) and (bigint.size() > 1)) {
        bigint.pop_back();
    }
}

bool operator<=(const BigInteger &first, const BigInteger &second) {
    return (first < second || second == first);
}

bool operator>=(const BigInteger &first, const BigInteger &second) {
    return (!(first < second));
}

bool operator>(const BigInteger &first, const BigInteger &second) {
    return !(first <= second);
}

BigInteger::BigInteger(int value) {
    if (value == 0) bigint.push_back(0);
    if (value >= 0) sign = 1;
    else sign = -1, value *= -1;
    while (value > 0) {
        bigint.push_back(value % BASE);
        value /= BASE;
    }
}

string BigInteger::toString() const {
    string str;
    if (sign == -1) str += '-';
    for (int i = static_cast<int>(bigint.size() - 1); i >= 0; --i) {
        string ans = to_string(bigint[i]);
        if ((i != static_cast<int>(bigint.size() - 1)) and (ans.length() < 9)){
            for (size_t j = 0; j < (9 - ans.length()); ++j) str.push_back('0');
        }
        str += ans;
    }
    return str;
}

ostream &operator<<(ostream &out, const BigInteger &data) {
    out << data.toString();
    return out;
}

BigInteger &BigInteger::operator=(const BigInteger &other) {
    BigInteger copy = other;
    swap(sign, copy.sign);
    swap(bigint, copy.bigint);
    return *this;
}

bool operator<(const BigInteger &first, const BigInteger &second) {
    if (first == second) return false;
    if (first.sign > second.sign) return false;
    else if (first.sign < second.sign) return true;
    if (first.sign == -1) return (-second < -first);
    if (second.sign < 0) return false;
    if (first.bigint.size() > second.bigint.size()) return false;
    if (first.bigint.size() < second.bigint.size()) return true;
    for (int i = first.bigint.size() - 1; i >= 0; --i){
        if (first.bigint[i] < second.bigint[i]) return true;
        if (first.bigint[i] > second.bigint[i]) return false;
    }
    return false;
}
bool operator!=(const BigInteger &first, const BigInteger &second) {
    return (!(first == second));
}

BigInteger BigInteger::operator-() const {
    BigInteger copy = BigInteger(*this);
    if (copy != 0) copy.sign *= -1;
    return copy;
}

BigInteger::BigInteger() : sign(1) {}


BigInteger &BigInteger::operator+=(const BigInteger &value) {
    int per = 0;
    if (sign == value.sign) {
        size_t ind = 0;
        while (ind < std::min(bigint.size(), value.bigint.size())) {
            if (bigint[ind] + per + value.bigint[ind] - BASE < 0) bigint[ind] += per + value.bigint[ind], per = 0;
            else bigint[ind] = bigint[ind] + per + value.bigint[ind] - BASE, per = 1;
            ++ind;
        }
        if (bigint.size() < value.bigint.size()) {
            for (size_t i = ind; i < value.bigint.size(); ++i) {
                if (value.bigint[i] + per > BASE) bigint.push_back(value.bigint[i] + per - BASE), per = 1;
                else bigint.push_back(value.bigint[i] + per), per = 0;
            }
        }
        else if (per > 0) {
            if (bigint.size() > value.bigint.size()) {
                while (bigint.size() > ind && per > 0) {
                    bigint[ind] = bigint[ind] + per - BASE;
                    per = 1;
                    ++ind;
                }
                if (bigint.size() == ind) bigint.push_back(per);
            }
            else bigint.push_back(per);
        }
    }
    else {
        if (sign == -1 && value.sign == -1) {
            sign = 1;
            *this += (value);
            sign = -1;
        }
        else if (sign == 1 && value.sign == -1) {
            if (*this >= -value) {

                for (size_t i = 0; i < value.bigint.size() || per != 0; ++i) {
                    int carry =  (i < value.bigint.size() ? value.bigint[i] : 0);
                    bigint[i] -= per + carry;
                    per = 0 > bigint[i];

                    if (per != 0) bigint[i] += BASE;
                }
            }

            else *this = -(-value += -*this);

            zeros();
        }

        else if (sign == -1 && value.sign == 1) {
            sign = 1;
            *this += -value;
            sign *= -1;
            zeros();

            if (bigint.size() == 1 && bigint[0] == 0) sign = 1;
        }
    }
    return *this;
}

BigInteger &BigInteger::operator-=(const BigInteger &value) {
    return *this += (-value);
}

BigInteger &BigInteger::operator++() {
    (*this) += 1;
    return *this;
}

BigInteger &BigInteger::operator--() {
    (*this) -= 1;
    return *this;
}

BigInteger BigInteger::operator++(int) {
    BigInteger copy = *this;
    *this += 1;
    return copy;
}

BigInteger BigInteger::operator--(int) {
    BigInteger copy = *this;
    *this -= 1;
    return copy;
}

void BigInteger::shift() {
    if (bigint.size() == 0) {
        bigint.push_back(0);
        return;
    }
    bigint.push_back(bigint[bigint.size() - 1]);
    for (int i = bigint.size() - 2; i > 0; --i) {
        bigint[i] = bigint[i - 1];
    }
    bigint[0] = 0;
}

BigInteger operator+(const BigInteger &first, const BigInteger &second) {
    BigInteger copy = first;
    copy += second;
    return copy;
}

BigInteger operator-(const BigInteger &first, const BigInteger &second) {
    BigInteger copy = first;
    return (copy -= second);
}

BigInteger &BigInteger::operator*=(const BigInteger &value) {
    sign *= value.sign;
    BigInteger copy = *this;

    copy.bigint.assign(bigint.size() + value.bigint.size(), 0);

    long long per, sum, carry;
    for (size_t i = 0; i < bigint.size(); ++i){
        per = 0;
        for (size_t j = 0; j < value.bigint.size() || per != 0; ++j) {
            carry =  (j < value.bigint.size() ? value.bigint[j] : 0);
            sum = copy.bigint[i + j] + bigint[i] * 1LL * carry + per;
            copy.bigint[i + j] = (sum % BASE);
            per = (sum / BASE);
        }
    }
    *this = copy;

    zeros();
    return *this;
}

BigInteger operator*(const BigInteger &first, const BigInteger &second) {
    BigInteger copy = first;
    return (copy *= second);
}

BigInteger &BigInteger::operator/=(const BigInteger &value) {
    sign *= value.sign;
    BigInteger copy, ans;
    if (value.sign == 1) copy = value;
    else copy = -value;

    for (int i = bigint.size() - 1; i > -1; --i){

        int x = 0, left = 0, mid, right = BASE;
        ans.shift();
        ans.bigint[0] = bigint[i];
        ans.zeros();

        while (left <= right) {
            mid = (left + right) / 2;
            BigInteger res = copy * mid;
            if (res > ans) right = mid - 1;
            else {
                x = mid;
                left = mid + 1;
            }
        }

        bigint[i] = x;
        ans -= (copy * x);
    }
    if (*this == 0) sign = 1;
    zeros();
    Sign();
    return *this;
}

BigInteger operator/(const BigInteger &first, const BigInteger &second) {
    BigInteger copy = first;
    return (copy /= second);
}

BigInteger::operator bool() const {
    if (bigint.size() == 0 || (bigint.size() == 1 && bigint[0] == 0)) return false;
    return true;
}
BigInteger &BigInteger::operator%=(const BigInteger &value) {
    *this -= (*this / value) * value;
    return *this;
}

void BigInteger::Sign() {
    if (bigint.size() == 1 && bigint[0] == 0) sign = 1;
}

BigInteger operator%(const BigInteger &first, const BigInteger &second) {
    BigInteger copy = first;
    return (copy %=second);
}

class Rational {

    friend bool operator==(const Rational &first, const Rational &second);

    friend bool operator<(const Rational &first, const Rational &second);

private:
    BigInteger num;

    BigInteger den;

    void reduction();

    int sign;

public:
    Rational() = default;

    Rational(int value);

    Rational(const BigInteger &value);

    string toString();

    Rational operator-() const;

    void normsign();


    BigInteger gcd(const BigInteger &first, const BigInteger &second);

    Rational & operator++();

    Rational &operator--();

    Rational &operator++(int);

    Rational &operator--(int);

    Rational operator+=(const Rational& value);

    Rational & operator*=(const Rational &value);

    Rational operator-=(const Rational& value);

    Rational & operator/=(const Rational &value);

    string asDecimal(size_t precision) const;
};

Rational::Rational(int value) {
    den = 1;
    if (value < 0) sign = -1, num = -value;
    else sign = 1, num = value;
}

Rational::Rational(const BigInteger &value) {
    if (value < 0) sign = -1, num = -value;
    else sign = 1, num = value;
    den = 1;
}

string Rational::toString() {
    string str;
    if (sign == -1) str += '-';
    str += num.toString();
    if (den != 1) {
        str += '/' + den.toString();
    }
    return str;
}

bool operator==(const Rational &first, const Rational &second) {
    if ((first.sign * first.num == 0 && second.sign * second.num == 0) || (first.sign * first.num == second.sign * second.num && first.den == second.den)) return true;
    return false;
}

bool operator<(const Rational &first, const Rational &second) {
    if (first.sign * first.num * second.den < second.sign * second.num * first.den) return true;
    return false;
}

Rational Rational::operator-()  const {
    Rational copy;
    copy.sign = -sign;
    copy.num = num;
    copy.den = den;
    return copy;
}

void Rational::normsign() {
    if (num < 0) {
        num = -num;
        sign *= -1;
    }
    if (num == 0) sign = 1;
}


bool operator>(const Rational &first, const Rational &second) {
    return second < first;
}

bool operator!=(const Rational &first, const Rational &second) {
    return !(first == second);
}

bool operator<=(const Rational &first, const Rational &second) {
    return !(first > second);
}

bool operator>=(const Rational &first, const Rational &second) {
    return  !(first < second);
}

Rational Rational::operator+=(const Rational& value) {
    num = sign * num * value.den + value.sign * value.num * den;
    den = den * value.den;
    normsign();
    reduction();
    return *this;

}

void Rational::reduction() {
    BigInteger common = gcd(num, den);
    if (common != 1) num /= common, den /= common;
    normsign();
}


Rational &Rational::operator--(int) {
    *this -= 1;
    normsign();
    return *this;
}
BigInteger Rational::gcd(const BigInteger &first, const BigInteger &second) {
    if (second == 0) return first;
    return gcd(second, first % second);
}

Rational Rational::operator-=(const Rational& value) {
    *this += (-value);
    normsign();
    return *this;
}

Rational &Rational::operator++() {
    *this += 1;
    return *this;
}

Rational &Rational::operator++(int) {
    *this += 1;
    return *this;
}

Rational operator-(const Rational &first, const Rational &second) {
    Rational copy = first;
    copy -= second;
    copy.normsign();
    return copy;
}


Rational operator+(const Rational &first, const Rational &second) {
    Rational copy = first;
    copy += second;
    copy.normsign();
    return copy;
}

Rational &Rational::operator*=(const Rational &value) {
    num *= value.num;
    den *= value.den;
    sign *= value.sign;
    normsign();
    reduction();
    return *this;
}

Rational operator*(const Rational &first, const Rational &second) {
    Rational copy = first;
    copy *= second;
    copy.normsign();
    return copy;
}

Rational &Rational::operator/=(const Rational &value) {
    num *= value.den;
    den *= value.num;
    sign *= value.sign;
    normsign();
    reduction();
    return *this;
}

Rational operator/(const Rational &first, const Rational &second) {
    Rational copy = first;
    copy /= second;
    copy.normsign();
    return copy;
}

string Rational::asDecimal(size_t precision) const {
    string ans, nul;
    BigInteger copy = num;
    if (num < 0) {
        copy = -copy;
    }

    for (size_t i = 0; i < precision; ++i) copy *= 10;

    ans += (copy / den).toString();

    while (ans.size() < precision - nul.size() + 1) {
        nul += '0';
    }

    ans = nul + ans;

    if (sign < 0) ans = '-' + ans;
    ans.insert(ans.size() - precision, ".");
    return ans;
}

Rational &Rational::operator--() {
    *this -= 1;
    normsign();
    return *this;
}
