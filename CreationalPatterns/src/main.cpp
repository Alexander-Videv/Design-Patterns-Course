#include "../include/Factory.hpp"
#include "../include/Figure.hpp"

int main()
{
    AbstractFigureFactory abstract_fact;

    // std::unique_ptr<FigureFactory> console_fact = abstract_fact.create("console");

    // if (!console_fact)
    //     return 1;

    // std::unique_ptr<Figure> my_fig = console_fact->create();

    // if (!my_fig)
    //     return 1;

    // std::unique_ptr<Figure> my_fig2 = StringToFigure::create_from("circle 45 rectangle 88 77");

    // std::cout << *my_fig2 << "\n";

    RandomFactory rand_fact(0, 3000, 4000);

    for (size_t i = 0; i < 10; i++)
    {
        std::cout << rand_fact.create()->to_string() << "\n";
    }

    std::unique_ptr<FigureFactory> file_fact = abstract_fact.create("file /home/sashos/MyFiles/my_shapes.txt");

    if (!file_fact)
        return 1;

    std::unique_ptr<Figure> shape;

    for (size_t i = 0; i < 10; i++)
    {
        shape = file_fact->create();
        if (shape)
            std::cout << i << " " << *shape << "\n";
    }

    return 0;
}