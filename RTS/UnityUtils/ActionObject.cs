using UnityEngine;

namespace ZG.RTS
{
    public abstract class ActionObject : ScriptableObject
    {
        [SerializeField]
        internal ActionObject[] _children;

        private Action __instance;

        public Action instance
        {
            get
            {
                if (__instance == null)
                {
                    int numChildren = _children == null ? 0 : _children.Length;
                    __instance = _Create(numChildren);

                    ActionObject child;
                    for (int i = 0; i < numChildren; ++i)
                    {
                        child = _children[i];

                        __instance.Set(child == null ? null : child.instance, i);
                    }
                }

                return __instance;
            }
        }

        protected abstract Action _Create(int numChildren);
    }
}