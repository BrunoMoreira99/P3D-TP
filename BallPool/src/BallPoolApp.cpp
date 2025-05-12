#include <Roose/Core/EntryPoint.h>

#include "BallPoolLayer.h"

class BallPoolApp : public Roose::Application
{
public:
    BallPoolApp(const Roose::ApplicationSpecification& specification)
        : Application(specification)
    {
        PushLayer(new BallPoolLayer());
    }
};

Roose::Application* Roose::CreateApplication(const ApplicationCommandLineArgs args)
{
    ApplicationSpecification spec;
    spec.Name = "Ball Pool";
    spec.CommandLineArgs = args;

    return new BallPoolApp(spec);
}
