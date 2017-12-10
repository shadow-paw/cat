#ifndef __CAT_UI_INTERPOLATOR_H__
#define __CAT_UI_INTERPOLATOR_H__

namespace cat {
// ----------------------------------------------------------------------------
//! Animation Interpolator Interface
class Interpolator {
public:
    //! map input value to interpolated value
    //! \param input 0~1, inclusive
    //! \return interpolated value, 0~1, inclusive
    virtual float get(float input) = 0;
};
// ----------------------------------------------------------------------------
class LinearInterpolator : public Interpolator {
public:
    virtual float get(float input) { return input; }
};
// ----------------------------------------------------------------------------
class AccelerateInterpolator : public Interpolator {
public:
    virtual float get(float input) { return input * input; }
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_UI_INTERPOLATOR_H__
