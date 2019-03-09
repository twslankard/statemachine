#ifndef LCC_IMMUTABLE_H
#define LCC_IMMUTABLE_H

template<typename T>
class AbstractImmutable {
    private:
        const T _value;
    public:
        AbstractImmutable(const T & value) : _value(value) {}
        const T & getValue() const { return _value; }

        inline friend bool operator<(const AbstractImmutable<T> & a, const AbstractImmutable<T> & b) {
            return a.getValue() < b.getValue();
        }

        inline friend bool operator>(const AbstractImmutable<T> & a, const AbstractImmutable<T> & b) {
            return a.getValue() > b.getValue();
        }

        inline friend bool operator==(const AbstractImmutable<T> & a, const AbstractImmutable<T> & b) {
            return a.getValue() == b.getValue();
        }

        const AbstractImmutable<T> operator+(const AbstractImmutable<T> & that) const {
            return AbstractImmutable<T>(getValue() + that.getValue());
        }

        const AbstractImmutable<T> operator-(const AbstractImmutable<T> & that) const {
            return AbstractImmutable<T>(getValue() - that.getValue());
        }

        const AbstractImmutable<T> operator/(const AbstractImmutable<T> & that) const {
            return AbstractImmutable<T>(getValue() / that.getValue());
        }

        const AbstractImmutable<T> operator*(const AbstractImmutable<T> & that) const {
            return AbstractImmutable<T>(getValue() * that.getValue());
        }

        inline friend std::ostream & operator<<(std::ostream & out, const AbstractImmutable<T> & that) {
            out << that.getValue();
            return out;
        }
};

template<typename T>
class ImmutableString : public AbstractImmutable<std::string> {
    using AbstractImmutable::AbstractImmutable;
};

#endif

