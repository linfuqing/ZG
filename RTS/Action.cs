using System;
using System.Collections.Generic;

namespace ZG.RTS
{
    /// <summary>
    /// 技能，包括普攻，伤害，治疗等。
    /// </summary>
    public class Action
    {
        private static Dictionary<IntPtr, Action> __actions = null;

        private IntPtr __instance;

        private Action[] __children;

#if DEBUG
        private int __index;

        private static int __count = 0;
        private const string __NAME = "pAction";

        public string name
        {
            get
            {
                return __NAME + __index;
            }
        }
#endif

        internal IntPtr instance
        {
            get
            {
                return __instance;
            }
        }

        internal Action(IntPtr instance, int childCount)
        {
#if DEBUG
            __index = __count++;
            Lib.LogVar("LPZGTILEOBJECTACTION", name);
#endif
            
            __children = new Action[childCount];

            if (__actions == null)
                __actions = new Dictionary<IntPtr, Action>();

            __actions.Add(instance, this);

            __instance = instance;
        }

        ~Action()
        {

#if DEBUG
            Lib.LogCall(null, "ZGRTSDestroy", name);
#endif

            Lib.ZGRTSDestroy(__instance);
        }

        public bool Set(Action child, int index)
        {

#if DEBUG
            Lib.LogCall(null, "ZGRTSSetChildToAction", name, child == null ? IntPtr.Zero.ToString() : child.name, (uint)index);
#endif

            if (Lib.ZGRTSSetChildToAction(__instance, child == null ? IntPtr.Zero : child.__instance, (uint)index) == 0)
                return false;

            __children[index] = child;

            return true;
        }

        internal static Action Get(IntPtr instance)
        {
            if (__actions == null)
                return null;

            Action result;
            if (__actions.TryGetValue(instance, out result))
                return result;

            return null;
        }
    }
}