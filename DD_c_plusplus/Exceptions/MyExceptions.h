#ifndef TEMPLATETYPE_MYEXCEPTIONS_H
#define TEMPLATETYPE_MYEXCEPTIONS_H

#include <stdexcept>
#include <string>

using namespace std;

/**
 * @brief Exception thrown when the objective function has not been set.
 */
class ObjectiveFunctionNotExecuted : public exception {
public:
    ObjectiveFunctionNotExecuted(const char* message = "The Objective Function has not been executed yet");
    const char* what() const noexcept override;

private:
    const char* message;
};

/**
 * @brief Exception thrown when an unimplemented method is called.
 */
class NotImplementedError : public exception {
public:
    NotImplementedError(const char* method_name);
    const char* what() const noexcept override;

private:
    string message;
};

/**
 * @brief Exception thrown when the algorithm used to solve the problem has not been set.
 */
class ParametersNotSetException : public exception {
public:
    ParametersNotSetException(const char* message = "Objective has not been set, please use solve()");
    const char* what() const noexcept override;

private:
    const char* message;
};

/**
 * @brief Exception thrown when no cuts were found.
 */
class NoCutsFound : public exception {
public:
    NoCutsFound(const char* message = "No cuts were found");
    const char* what() const noexcept override;

private:
    const char* message;
};

/**
 * @brief Exception thrown when no cut strengthening was found.
 */
class NoCutStrengthening : public exception {
public:
    NoCutStrengthening(const char* message = "No strengthened cut was found");
    const char* what() const noexcept override;

private:
    const char* message;
};

/**
 * @brief Exception thrown when the max flow has not been executed yet.
 */
class MaxFlowNotExecuted : public exception {
public:
    MaxFlowNotExecuted(const char* message = "Max Flow has not been executed yet");
    const char* what() const noexcept override;

private:
    const char* message; // Error message
};

class NoGraphFound : public exception {
public:
    NoGraphFound(const char* message = "No graph was found, please create a graph first");
    const char* what() const noexcept override;
private:
    const char* message; // Error message
};

class GraphAlreadyExists : public exception {
public:
    GraphAlreadyExists(const char* message = "A graph already exists, you can't create another one");
    const char* what() const noexcept override;
private:
    const char* message;

};

#endif //TEMPLATETYPE_MYEXCEPTIONS_H
