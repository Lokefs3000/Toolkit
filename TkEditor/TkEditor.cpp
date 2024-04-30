#include <Application.h>
#include <ApplicationLayer.h>

typedef void(__stdcall* editor_main_init_fn)();
typedef void(__stdcall* editor_main_update_fn)();

class EditorLayer : public ApplicationLayer
{
private:
    editor_main_init_fn m_Init;
    editor_main_update_fn m_Update;
public:
    using ApplicationLayer::ApplicationLayer;

    void Init() override
    {
        LoadScriptAssembly("script/Toolkit.Editor.dll");

        m_Init = (editor_main_init_fn)GetScriptFunction("Toolkit.Editor", "Toolkit.Editor.EditorMain", "GlInit");
        m_Update = (editor_main_update_fn)GetScriptFunction("Toolkit.Editor", "Toolkit.Editor.EditorMain", "GlUpdate");

        m_Init();
    }

    void Update() override
    {
        m_Update();
    }
};

int main()
{
    Application app;
    app.AddLayer<EditorLayer>();
    app.Run();
}
