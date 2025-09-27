//Bibliotecas OpenGL para manejar gráficos y ventanas
#include <glew.h>
#include <glfw3.h>
//Librerías estándar para poder manejar vectores, entrada/salida y formato.
#include <vector>
#include <iostream>
#include <limits>
#include <cmath>
#include <string>

// Compilar (ejemplo, Linux/MinGW):
// g++ -o circunf circunf.cpp -lGL -lGLEW -lglfw


#include <algorithm>
#include <iomanip>

//Se define  un punto 2D con coordenadas enteras x y

struct Point {
    int x, y;
}

;

//la función "expande" un punto del octante a los 8 puntos 
// simétricos que completan el círculo sin tener que volver a calcular cada uno individualmente. 
// Es una manera rápida de aprovechar la simetría del círculo para ahorrar cálculos.

// Añade puntos simétricos en 8 octantes
void plotCirclePoints(int xc, int yc, int x, int y, std::vector<Point>& points) {
    points.push_back({ xc + x, yc + y });
    points.push_back({ xc - x, yc + y });
    points.push_back({ xc + x, yc - y });
    points.push_back({ xc - x, yc - y });
    points.push_back({ xc + y, yc + x });
    points.push_back({ xc - y, yc + x });
    points.push_back({ xc + y, yc - x });
    points.push_back({ xc - y, yc - x });
}

// Bresenham para circunferencia (enteros)
std::vector<Point> bresenhamCircle(int xc, int yc, int r) {
    std::vector<Point> points;
    if (r <= 0) return points;
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;
    plotCirclePoints(xc, yc, x, y, points);
    while (y >= x) {
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        }
        else {
            d = d + 4 * x + 6;
        }
        plotCirclePoints(xc, yc, x, y, points);
    }
    return points;
}

//Dibujar ejes X Y en color verde 
void drawAxes(int width, int height) {
    glColor3f(0.0f, 1.0f, 0.0f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    // Eje X
    glVertex2i(0, height / 2);
    glVertex2i(width, height / 2);
    // Eje Y
    glVertex2i(width / 2, 0);
    glVertex2i(width / 2, height);
    glEnd();
}

//Interaccion con el usuario
int main() {
    std::cout << "Algoritmo de Bresenham\n";
    std::cout << "Ingrese el radio (entero mayor que 0): ";

    int userRadius;
    while (true) {
        std::cin >> userRadius;
        if (std::cin.fail() || userRadius <= 0) {
            std::cout << "Por favor ingrese un valor entero positivo mayor que 0: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        else {
            break;
        }
    }

    // Diámetro calculo
    int desiredDiameter = 2 * userRadius;

   // Queremos que el diámetro que dibujamos(drawDiameter) sea 3 / 4 partes del tamaño total de la ventana(windowSize).
        // Para asegurarnos que se mantenga esa proporción exacta y que ambos sean números enteros,
        // usamos una fórmula con múltiplos de un mismo número m:
        // drawDiameter será igual a 6 veces m,
        // windowSize será igual a 8 veces m,
        // así la proporción drawDiameter/windowSize será exactamente 6m/8m = 3/4.

        // Calculamos un valor m basado en el diámetro que el usuario pidió dividido entre 6.
        // Redondeamos m para que sea entero y si es menor que 1, lo ponemos en 1 para evitar tamaños muy pequeños.
    double m_double = desiredDiameter / 6.0;
    int m = static_cast<int>(std::lround(m_double));
    if (m < 1) m = 1;

    // Limites para la ventana
    const int MIN_WINDOW = 200;
    const int MAX_WINDOW = 1600;

    // Ajustar m si con el valor actual la ventana estaría fuera de los límites
    if (8 * m < MIN_WINDOW) m = static_cast<int>(std::ceil(MIN_WINDOW / 8.0));
    if (8 * m > MAX_WINDOW) m = static_cast<int>(std::floor(MAX_WINDOW / 8.0));
    if (m < 1) m = 1;

    // Ya con m ajustado definimos:
// drawDiameter: tamaño del diámetro que vamos a dibujar, múltiplo de 6 para que el radio sea entero.
// drawRadius: la mitad del diámetro, será el radio efectivo para el dibujo.
// windowSize: tamaño real de la ventana (ancho y alto), múltiplo de 8 para mantener la proporción con el diámetro.
    int drawDiameter = 6 * m;     
    int drawRadius = drawDiameter / 2; 
    int windowSize = 8 * m;       

    std::cout << "Radio ingresado: " << userRadius << "\n";
    std::cout << "Diametro solicitado: " << desiredDiameter << " pixels\n";
    std::cout << "Ajuste aplicado para lograr ratio EXACTA 3/4 usando múltiplos:\n";
    std::cout << "  Diametro usado: " << drawDiameter << " (m = " << m << ")\n";
    std::cout << "  Radio usado (entero): " << drawRadius << "\n";
    std::cout << "  Tamaño de ventana: " << windowSize << "x" << windowSize << "\n";
    std::cout << std::fixed << std::setprecision(5);
    std::cout << "Proporcion (diametro/ventana): " << drawDiameter << "/" << windowSize
        << " = " << static_cast<float>(drawDiameter) / windowSize << " (deberia ser 0.75 = 3/4)\n";

    if (!glfwInit()) {
        std::cerr << "Error iniciando GLFW\n";
        return -1;
    }

    std::string windowTitle = "Circunferencia - Radio usado: " + std::to_string(drawRadius) +
        " - Ventana: " + std::to_string(windowSize) + "x" + std::to_string(windowSize);

    GLFWwindow* window = glfwCreateWindow(windowSize, windowSize, windowTitle.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "Error creando ventana GLFW\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Error inicializando GLEW\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glPointSize(2.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        int fbw, fbh;
        glfwGetFramebufferSize(window, &fbw, &fbh);

        glViewport(0, 0, fbw, fbh);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, fbw, 0, fbh, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glClear(GL_COLOR_BUFFER_BIT);

        int xc = fbw / 2;
        int yc = fbh / 2;

        // Ejes
        drawAxes(fbw, fbh);

        // Generar puntos con el radio ajustado que garantiza la proporción exacta
        std::vector<Point> circlePoints = bresenhamCircle(xc, yc, drawRadius);

        // Dibujar circunferencia (puntos blancos)
        glPointSize(2.0f);
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_POINTS);
        for (const auto& p : circlePoints) {
            glVertex2i(p.x, p.y);
        }
        glEnd();

        // Punto central en rojo
        glPointSize(6.0f);
        glColor3f(1.0f, 0.0f, 0.0f);
        glBegin(GL_POINTS);
        glVertex2i(xc, yc);
        glEnd();

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
