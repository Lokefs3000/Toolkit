using Toolkit.Renderer;
using Toolkit.Core.Types;

namespace Toolkit.Editor.Interface
{
    internal class UICore
    {
        public static UICore Instance;

        public UICore()
        {
            Instance = this;

            Graphics.ClearColor = new Color4(0.2f, 0.2f, 0.2f, 1.0f);
        }
    }
}
