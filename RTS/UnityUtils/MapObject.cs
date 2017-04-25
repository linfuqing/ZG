using UnityEngine;

namespace ZG.RTS
{
    public class MapObject : ScriptableObject
    {

        [SerializeField]
        internal bool _isOblique;

        [SerializeField]
        internal int _width;
        [SerializeField]
        internal int _height;
        [SerializeField]
        internal int _depth = 1;

        [SerializeField]
        internal Vector2 _scale;

        private Map __instance;
        private bool __isDirty;

        public int width
        {
            get
            {
                return _width;
            }
        }

        public int height
        {
            get
            {
                return _height;
            }
        }

        public int depth
        {
            get
            {
                return _depth;
            }
        }

        public Vector2 scale
        {
            get
            {
                return _scale;
            }
        }

        public virtual Map instance
        {
            get
            {
                if (__instance == null || __isDirty)
                {
                    __instance = new Map(_width, _height, _depth, _isOblique);

                    __isDirty = false;
                }

                return __instance;
            }
        }
    }
}