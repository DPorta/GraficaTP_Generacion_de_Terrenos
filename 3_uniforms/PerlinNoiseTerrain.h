#include <cmath>
#include <iomanip>
#include <glutil.h>
class PerlinNoise
{
public:
    PerlinNoise() {};
    ~PerlinNoise() {};

    //FUNCIONES PARA LA GENERACION DE RUIDO DE PERLIN (PERLIN NOISE)

    //lerp devuelve un punto entre el valor inicial y final dependiendo de la velocidad/sensibilidad/peso
    double lerp(double inicio, double fin, double peso) { return (1.0 - peso) * inicio + peso * fin; }

    double dotGridGradient(int ix, int iy, double x, double y)
    {
        // distancias
        double dx = x - ix;
        double dy = y - iy;
        //retorna producto punto o escalar
        return (dx * gradient[iy][ix][0] + dy * gradient[iy][ix][1]);
    }

    double perlinNoise(double x, double y) {
        // coordenadas de la celda de la cuadricula
        int x0 = std::floor(x);
        int x1 = x0 + 1;
        int y0 = std::floor(y);
        int y1 = y0 + 1;

        // peso de la interpolacion
        auto sx = x - x0;
        auto sy = y - y0;

        // interpolacion entre gradientes de puntos de cuadricula
        auto n0 = dotGridGradient(x0, y0, x, y);
        auto n1 = dotGridGradient(x1, y0, x, y);
        auto ix0 = lerp(n0, n1, sx);
        n0 = dotGridGradient(x0, y1, x, y);
        n1 = dotGridGradient(x1, y1, x, y);
        auto ix1 = lerp(n0, n1, sx);
        auto value = lerp(ix0, ix1, sy);

        return value;
    }

};

class MyTerrain
{
    PerlinNoise* myPerlin;
public:
    MyTerrain() {
        myPerlin = new PerlinNoise();
    };
    ~MyTerrain() {};

    //FUNCIONES PARA GENERAR EL TERRENO USANDO PERLIN NOISE

    //genera la matriz de grandientes (pendientes) para luego ser procesadas con perlin noise
    using uint = unsigned int;
    template <uint rows, uint columns>
    void genGradientMatrix(double(&grad)[rows][columns][3]) {
        srand(time(0));
        for (auto i = 0u; i < rows; ++i) {
            for (auto j = 0u; j < columns; ++j) {
                grad[i][j][0] = rand() % 3;
                grad[i][j][1] = rand() % 3;
            }
        }
    }

    //genera el terreno usando la funcion perlin noise
    template <uint rows, uint columns>
    void genTerrain(double(&terrain)[rows][columns]) {
        genGradientMatrix(gradient);
        for (auto i = 0u; i < rows; ++i) {
            for (auto j = 0u; j < columns; ++j) {
                terrain[i][j] = myPerlin->perlinNoise(i * 0.135f, j * 0.135) + 0.50;
            }
        }
    }

    //aplica funcion de transicion para el suavizado del terreno
    template <uint rows, uint columns>
    void genTerrainTransitions(double(&terreno)[rows][columns], std::vector<glm::vec4>& transition, u32 y_level) {
        for (auto i = 0u; i < rows; ++i) {
            for (auto j = 0u; j < columns; ++j) {
                terreno[i][j] *= y_level;
                terreno[i][j] = std::round(terreno[i][j]);
            }
        }
        for (auto i = 0u; i < rows; ++i) {
            for (auto j = 0u; j < columns; ++j) {
                for (auto h = 0; h < terreno[i][j]; ++h) {
                    transition.push_back(glm::vec4(i * 0.5f, h / 2.0, j * 0.5f, h == (terreno[i][j] - 1)));
                }
            }
        }
    }

};


