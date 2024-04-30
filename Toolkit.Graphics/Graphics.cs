using Toolkit.Core.Types;

namespace Toolkit.Renderer
{
    public static class Graphics
    {
        public static Color4 ClearColor
        {
            get { return _clearColor; }
            set { SetClearColor(value); }
        }

        internal static Color4 _clearColor;

        internal static void SetClearColor(Color4 value)
        {
            _clearColor = value;
            unsafe { _setClearColor(value); };
        }

        private static unsafe delegate*<Color4, void> _setClearColor;
    }
}
