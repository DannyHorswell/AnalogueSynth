#pragma once

#include <stdint.h>
#include <iostream>

extern const int32_t multDivider;
extern const int32_t neg_multDivider;
extern const float f_multDivider;

class signal
{
    int64_t _value;

public:

    // Ctors
    inline signal()
    {
        _value = 0;
    }

    inline signal(const signal& value)
    {
        _value = value._value;
    }

    inline signal(const int& value)
    {
        _value = (int64_t) value * multDivider;
    }
/*
    inline signal(int32_t value)
    {
        _value = value;
    }

    inline signal(int64_t value)
    {
        _value = value;
    }*/

    inline signal(const float& value)
    {
        _value = (int64_t) (value * f_multDivider);
    }

    // Conversions
    inline float ToFloat() const
    {
        return ((float)_value) / multDivider;
    }

    inline int32_t ToInt() const 
    {
        return (int32_t)_value;
    }

    inline int32_t ToIntLimit() const 
    {
        if (_value > multDivider)
        {
            return multDivider;
        }

        if (_value < neg_multDivider)
        {
            return neg_multDivider;
        }

        return (int32_t)_value;
    }

    inline int64_t ToLong() const 
    {
        return _value;
    }

    // Assign
    /*inline signal& operator=(const int& val2)
    {
        _value = val2 * multDivider;
        return *this;
    }

    inline signal& operator=(const float& val2)
    {
        _value = val2 * multDivider;
        return *this;
    }*/

    inline signal& operator=(const signal& val2)
    {
        if (this == &val2) 
            return *this;

        _value = val2._value;
        return *this;
    }


    // Arithmetic modification
    inline signal& operator+= (const signal& val2)
    {
        _value = _value + val2._value;
        return *this;
    }

    inline signal& operator-= (const signal& val2)
    {
        _value = _value - val2._value;
        return *this;
    }

    inline signal& operator*= (const signal& val2)
    {
        _value = (_value * val2._value) / multDivider;
        return *this;
    }

    inline signal& operator/= (const signal& val2)
    {
        _value = (_value * multDivider) / val2._value;
        return *this;
    }

        // Arithmetic
    const inline signal operator+ (const signal& val2) const
    {
        signal ret = *this;
        ret += val2;
        return ret;
    }

    const inline signal operator- (const signal val2) const
    {
        signal ret = *this;
        ret -= val2;
        return ret;
    }

    const inline signal operator* (const signal val2) const
    {
        signal ret = *this;
        ret *= val2;
        return ret;
    }

    const inline signal operator/ (const signal val2) const
    {
        signal ret = *this;
        ret /= val2;
        return ret;
    }

    // Comparison
    inline bool operator== (const signal& val2) const
    {
        return _value == val2._value;
    }

    inline bool operator!=(const signal& val2) const
    {
        return !(*this == val2);
    }

    inline bool operator> (const signal& val2) const
    {
        return _value > val2._value;
    }

    inline bool operator< (const signal& val2) const
    {
        return _value < val2._value;
    }

    inline bool operator>= (const signal& val2) const
    {
        return _value >= val2._value;
    }

    inline bool operator<= (const signal& val2) const
    {
        return _value <= val2._value;
    }
};

extern signal sig_0; // = 0.0F;
extern signal sig_HALF; // = 0.5F;
extern signal sig_1; //= 1.0F;
extern signal sig_2; //= 2.0F;
extern signal sig_4; //= 4.0F;
extern signal sig_12; // = 12.0F;
extern signal sig_64; //= 64.0F;
extern signal sig_128; // = 128.0F;

extern signal sig_QUATER_PI; // = 1.57079632675F;
extern signal sig_HALF_PI; // = 1.57079632675F;
extern signal sig_PI; // = 3.1415926535F;
extern signal sig_TWO_PI; // = 6.2831853072F;

extern signal sig_NEG_1; // = -1.0F;

extern signal sig_B_; // = 1.2732395447F; //B = 4.0 / PI;
extern signal sig_C_; // _ = -0.405284735F; //C = -4.0 /(PI_SQUARED);

extern float PI; // = 3.1415926535F;
extern float TWO_PI; // = 6.2831853072F;

// Fast approximation for sin
inline signal wg_sin(signal x)
{
    signal temp;
    signal temp2;

    if (x > sig_PI)
    {
        x -= sig_TWO_PI;
    }

    temp = sig_B_ * x;
    temp2 = sig_C_ * x * x;

    float bBit = temp.ToFloat();
    float cBit = temp2.ToFloat();

    if (x > sig_0)
    {
        return temp + temp2;
    }
    else
    {
        return temp - temp2;
    }
}

extern signal lastTestPhase;

inline signal getNextTestValue()
{
	signal ret = sig_HALF * wg_sin(lastTestPhase);
	lastTestPhase += (sig_PI / 110);
	
	if (lastTestPhase > sig_PI)
	{
		lastTestPhase -= sig_TWO_PI;
	}
	
	return ret;
}

