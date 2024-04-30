using System;
using System.Collections.Generic;
using System.Linq;
using System.Linq.Expressions;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace Toolkit.Core.Interop
{
    internal class DelegateCreator
    {
        private static readonly Func<Type[], Type> MakeNewCustomDelegate = (Func<Type[], Type>)Delegate.CreateDelegate(typeof(Func<Type[], Type>), typeof(Expression).Assembly.GetType("System.Linq.Expressions.Compiler.DelegateHelpers").GetMethod("MakeNewCustomDelegate", BindingFlags.NonPublic | BindingFlags.Static));
        
        public static Type CreateDelegateType(Type ret, params Type[] parameters)
        {
            Type[] args = new Type[parameters.Length + 1];
            if (parameters.Length > 0)
                parameters.CopyTo(args, 0);
            args[args.Length - 1] = ret;
            return MakeNewCustomDelegate(args);
        }
    }
}
