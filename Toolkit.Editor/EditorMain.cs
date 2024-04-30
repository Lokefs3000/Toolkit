using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using Toolkit.Core;
using Toolkit.Editor.Interface;

namespace Toolkit.Editor
{
    public class EditorMain
    {
        public static EditorMain Instance;

        private UICore _uiCore;

        public EditorMain()
        {
            Logger.Log("Initialized editor..");

            _uiCore = new UICore();
        }

        public void Update()
        {

        }

        public static void GlInit()
        {
            Instance = new EditorMain();
        }

        public static void GlUpdate()
        {
            Instance.Update();
        }
    }
}
