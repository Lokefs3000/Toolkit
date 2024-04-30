using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Runtime.Loader;
using System.Text;
using System.Threading.Tasks;

namespace Toolkit.Core.Interop
{
    internal class AssemblyLoader
    {
        private static Dictionary<int, AssemblyLoadContext> _contexts = new Dictionary<int, AssemblyLoadContext>();
        private static Dictionary<int, Assembly> _assemblies = new Dictionary<int, Assembly>();
        private static Dictionary<int, int> _assemblyNameResolve = new Dictionary<int, int>();
        private static Dictionary<int, int> _assemblyTypes = new Dictionary<int, int>();

        static AssemblyLoader()
        {
            _assemblies.Add("Toolkit.Core".GetHashCode(), Assembly.GetExecutingAssembly());
            _assemblyNameResolve.Add(Assembly.GetExecutingAssembly().FullName.GetHashCode(), "Toolkit.Core".GetHashCode());

            AssemblyLoadContext.Default.Resolving += ResolveAssembly;
        }

        private static Assembly? ResolveAssembly(AssemblyLoadContext arg1, AssemblyName arg2)
        {
            if (_assemblyNameResolve.TryGetValue(arg2.FullName.GetHashCode(), out int key))
                return _assemblies[key];

            Logger.Error($"Failed to resolve assembly: {arg2.Name}!");
            return null;
        }

        [UnmanagedCallersOnly(EntryPoint = "AssemblyLoader_Load")]
        public static void Load(NativeString ctx, NativeString path)
        {
            string? contextname = ctx;

            if (contextname == null)
            {
                Logger.Error("Assembly loader context name cannot be null!");
                return;
            }

            string? filepath = path;

            if (filepath == null)
            {
                Logger.Error("Assembly file path cannot be null!");
                return;
            }
            
            int id = Path.GetFileNameWithoutExtension(filepath).GetHashCode();

            if (_assemblies.ContainsKey(id))
            {
                return;
            }

            /*if (!_contexts.TryGetValue(contextname.GetHashCode(), out AssemblyLoadContext? context))
            {
                context = new AssemblyLoadContext(contextname, true);
                _contexts.Add(contextname.GetHashCode(), context);
            }*/
            AssemblyLoadContext context = AssemblyLoadContext.Default;

            Assembly assembly = context.LoadFromAssemblyPath(filepath);
            _assemblies.Add(id, assembly);
            _assemblyNameResolve.Add(assembly.FullName.GetHashCode(), id);

            LoadTypesFromAssembly(assembly, id);

            Logger.Log($"Loaded {filepath}!");
        }

        [UnmanagedCallersOnly(EntryPoint = "AssemblyLoader_UnloadAll")]
        public static void UnloadAll()
        {
            foreach (KeyValuePair<int, AssemblyLoadContext> kvp in _contexts)
            {
                kvp.Value.Unload();
            }

            _assemblies.Clear();
            _contexts.Clear();
        }

        [UnmanagedCallersOnly(EntryPoint = "AssemblyLoader_GetMethod")]
        public static unsafe IntPtr GetMethod(NativeString nsdll, NativeString nstype, NativeString nsmethod)
        {
            string? sdll = nsdll.ToString();

            if (sdll == null)
            {
                Logger.Error("Cannot get method because dll is null!");
                return IntPtr.Zero;
            }

            int id = sdll.GetHashCode();

            if (!_assemblies.TryGetValue(id, out Assembly? value))
            {
                Logger.Error($"Cannot get dll: {sdll}");
                return IntPtr.Zero;
            }

            string? stype = nstype.ToString();

            if (stype == null)
            {
                Logger.Error("Cannot get method because type is null!");
                return IntPtr.Zero;
            }

            string? smethod = nsmethod.ToString();

            if (smethod == null)
            {
                Logger.Error("Cannot get method because method is null!");
                return IntPtr.Zero;
            }

            Type? type = value.GetType(stype, true, false);

            if (type == null)
            {
                Logger.Error("Failed to get method type!");
                return IntPtr.Zero;
            }

            MethodInfo? field = type.GetMethods(BindingFlags.Static | BindingFlags.Public).FirstOrDefault(m => m.Name == smethod);

            if (field == null)
            {
                Logger.Error("Failed to get method field!");
                return IntPtr.Zero;
            }

            Type delgType = DelegateCreator.CreateDelegateType(field.ReturnType, field.GetGenericArguments());
            Delegate delg = Delegate.CreateDelegate(delgType, field);

            KeepReference.Add(delg);
            return Marshal.GetFunctionPointerForDelegate(delg);
        }

        internal static void LoadTypesFromAssembly(Assembly assembly, int id)
        {
            foreach (Type type in assembly.GetTypes())
                _assemblyTypes.Add((type.FullName + ", " + assembly.GetName().Name).GetHashCode(), id);
        }

        public static Type? GetType(string name, bool throwOnError, bool ignoreCase)
        {
            if (_assemblyTypes.TryGetValue(name.GetHashCode(), out int assemblyId))
                return _assemblies[assemblyId].GetType(name, throwOnError, ignoreCase);
            return Type.GetType(name, throwOnError, ignoreCase);
        }
    }
}
