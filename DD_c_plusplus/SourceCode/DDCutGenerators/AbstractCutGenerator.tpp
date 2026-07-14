#include "AbstractCutGenerator.h"

template<typename T>
AbstractCutGenerator<T>::AbstractCutGenerator() = default;


template <typename T>
string AbstractCutGenerator<T>::get_inequality_string_representation() const {
    string inequality;
    for (size_t i = 0; i < coefficients.size(); ++i) {
        if (coefficients[i] < 1 + tolerance and coefficients[i] > 1 - tolerance) {
            if (!inequality.empty()) {
                inequality += " + ";
            }
            inequality += "x_" + to_string(i);
        } else if (coefficients[i] < -1 + tolerance and coefficients[i] > -1 - tolerance) {
            if (!inequality.empty()) {
                inequality += " + ";
            }
            inequality += "-x_" + to_string(i);
        } else if (coefficients[i] != 0) {
            if (!inequality.empty()) {
                inequality += " + ";
            }
            inequality += to_string(coefficients[i]) + "x_" + to_string(i);
        }
    }
    inequality += " <= " + to_string(constant);
    return inequality;
}