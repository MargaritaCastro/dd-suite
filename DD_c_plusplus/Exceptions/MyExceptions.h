#ifndef TEMPLATETYPE_MYEXCEPTIONS_H
#define TEMPLATETYPE_MYEXCEPTIONS_H

#include <stdexcept>
#include <string>

using namespace std;

/**
 * @brief Excepción lanzada cuando no se ha establecido la función objetivo.
 */
class ObjectiveFunctionNotExecuted : public exception {
public:
    ObjectiveFunctionNotExecuted(const char* message = "The Objective Function has not been executed yet");
    const char* what() const noexcept override;

private:
    const char* message;
};

/**
 * @brief Excepción lanzada cuando se intenta llamar a un método no implementado.
 */
class NotImplementedError : public exception {
public:
    NotImplementedError(const char* method_name);
    const char* what() const noexcept override;

private:
    string message;
};

/**
 * @brief Excepción lanzada cuando no se ha establecido el algoritmo para resolver el problema.
 */
class ParametersNotSetException : public exception {
public:
    ParametersNotSetException(const char* message = "Objective has not been set, please use solve()");
    const char* what() const noexcept override;

private:
    const char* message;
};

/**
 * @brief Excepción lanzada cuando no se han encontrado cortes.
 */
class NoCutsFound : public exception {
public:
    NoCutsFound(const char* message = "No cuts were found");
    const char* what() const noexcept override;

private:
    const char* message;
};

/**
 * @brief Excepción lanzada cuando no se ha encontrado un levantamiento de corte.
 */
class NoCutStrengthening : public exception {
public:
    NoCutStrengthening(const char* message = "No strengthened cut was found");
    const char* what() const noexcept override;

private:
    const char* message;
};

/**
 * @brief Excepción lanzada cuando no se ha encontrado un corte de Gomory.
 */
class MaxFlowNotExecuted : public exception {
public:
    MaxFlowNotExecuted(const char* message = "Max Flow has not been executed yet");
    const char* what() const noexcept override;

private:
    const char* message; // Mensaje de error
};

class NoGraphFound : public exception {
public:
    NoGraphFound(const char* message = "No graph was found, please create a graph first");
    const char* what() const noexcept override;
private:
    const char* message; // Mensaje de error
};

class GraphAlreadyExists : public exception {
public:
    GraphAlreadyExists(const char* message = "A graph already exists, you can't create another one");
    const char* what() const noexcept override;
private:
    const char* message;

};

#endif //TEMPLATETYPE_MYEXCEPTIONS_H
