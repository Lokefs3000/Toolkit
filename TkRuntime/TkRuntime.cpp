#include <Application.h>
#include <ApplicationLayer.h>

class RuntimeLayer : public ApplicationLayer
{
public:
    using ApplicationLayer::ApplicationLayer;

    void Init() override
    {
        
    }
};

int main()
{
    Application app;
    app.AddLayer<RuntimeLayer>();
    app.Run();
}
