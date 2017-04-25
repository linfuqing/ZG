using UnityEngine;
using UnityEngine.Assertions;

namespace ZG.RTS
{
    public class ObjectBehaviour : MonoBehaviour
    {
        public class Instance : Object
        {
            private ObjectBehaviour __parent;

            public ObjectBehaviour parent
            {
                get
                {
                    return __parent;
                }
            }

            public Instance(ObjectBehaviour parent, int width, int height, int offset) : base(width, height, offset)
            {
                __parent = parent;
            }
        }
        
        [SerializeField]
        internal int _width;
        [SerializeField]
        internal int _height;
        [SerializeField]
        internal int _offset;
        [SerializeField]
        internal int _camp;
        [SerializeField]
        internal int _label;
        [SerializeField]
        internal int _hp;
        [SerializeField]
        internal int _moveTime;
        [SerializeField]
        internal int _setTime;
        [SerializeField]
        internal int _handTime;
        [SerializeField]
        internal int _distance;
        [SerializeField]
        internal int _range;
        [SerializeField]
        internal int[] _attack;
        [SerializeField]
        internal int[] _defense;

        [SerializeField]
        internal ActionObject _actionObject;

        [SerializeField]
        internal MapBehaviour _map;

        [SerializeField]
        internal ManagerBehaviour _manager;
        
        private int __index;
        private int __areaIndex;
        private float __time;

        private Instance __instance;

        public Instance instance
        {
            get
            {
                return __instance;
            }
        }

        public MapBehaviour map
        {
            get
            {
                return _map;
            }
        }

        public ManagerBehaviour manager
        {
            get
            {
                return _manager;
            }

            set
            {
                Assert.IsNotNull(__instance);
                if (__instance == null)
                    return;

                __instance.manager = value == null ? null : value.instance;

                _manager = value;
            }
        }
        
        public int hp
        {
            get
            {
                return _hp;
            }
        }

        public void Awake()
        {
            Init();
        }

        public void Init()
        {
            __instance = new Instance(this, _width, _height, _offset);
            __instance.action = _actionObject == null ? null : _actionObject.instance;

            __instance.camp = _camp;
            __instance.label = _label;
            __instance.hp = _hp;
            __instance.moveTime = _moveTime;
            __instance.setTime = _setTime;
            __instance.handTime = _handTime;
            __instance.distance = _distance;
            __instance.range = _range;

            int count = _attack == null ? 0 : _attack.Length;
            for (int i = 0; i < count; ++i)
                __instance.SetAttack(i, _attack[i]);

            count = _defense == null ? 0 : _defense.Length;
            for (int i = 0; i < count; ++i)
                __instance.SetDefense(i, _defense[i]);
        }

        public bool Set(MapBehaviour map, int areaIndex)
        {
            if (__instance == null)
                return false;

            MapObject mapObject = map == null ? null : map.instance;
            if (mapObject == null)
                return false;

            int index = map.Get(transform.position, areaIndex);
            if (!__instance.Set(map == null || map.instance == null ? null : map.instance.instance, index))
                return false;

            __index = index;

            __areaIndex = areaIndex;

            _map = map;

            return true;
        }

        public bool Unset()
        {
            if (_map == null || __instance == null || !__instance.Unset())
                return false;

            __index = -1;

            __areaIndex = -1;

            _map = null;

            return true;
        }

        public bool Move(Vector3 direction, float time, float epsilon)
        {
            /*if (__instance == null)
                return false;

            Map map = _map == null || _map.instance == null ? null : _map.instance.instance;
            if (map == null)
                return false;
            
            float maxDistance = (1000.0f / instance.moveTime) * time;
            Vector3 position = transform.position;
            position += direction * maxDistance;
            int index = _map.Get(position);
            if (index != __mapIndex)
            {
                direction = -direction;
                float distance, minDistance = 0;
                while (!__instance.Set(map, index))
                {
                    index = _map.Get(position, ref direction, out distance);
                    minDistance += distance;
                    if (minDistance > maxDistance)
                    {
                        if (__instance.Set(map, __mapIndex))
                            return false;
                        else
                            break;
                    }
                    
                    position += direction * distance;
                }
                
                if(minDistance > 0.0f)
                {
                    Debug.Log(minDistance);
                }

                __mapIndex = index;
            }*/

            Vector3 position = transform.position;
            if (__Move(ref position, direction, (1000.0f / instance.moveTime) * time, epsilon))
            {
                transform.position = position;

                return true;
            }

            return false;
        }

        public bool Do(Vector3 position, ManagerBehaviour manager)
        {
            if (_map == null)
                return false;

            return __Do(_map.Get(position, __areaIndex), manager);
        }

        public bool Do(Vector3 direction, int count, ManagerBehaviour manager, out Vector3 position)
        {
            if(_map == null)
            {
                position = Vector3.zero;

                return false;
            }

            position = transform.position;
            float distance;
            int index = _map.Get(position, ref direction, out distance);
            for (int i = 0; i < count; ++ i)
            {
                position += direction * distance;
                index = _map.Get(position, ref direction, out distance);
            }

            return __Do(index, manager);
        }

        private bool __Do(int index, ManagerBehaviour manager)
        {
            if (__instance == null)
                return false;

            float time = Time.time, elapsedTime = time - __time;
            ByteArray<Info> infos;
            if (__instance.Do(Mathf.RoundToInt(elapsedTime * 1000.0f), index, manager == null ? null : manager.instance, out infos))
            {
                if(manager != null)
                    manager.Set(infos);

                __time = time;

                return true;
            }

            return false;
        }

        private bool __Move(ref Vector3 position, Vector3 direction, float maxDistance, float epsilon)
        {
            if (__instance == null)
                return false;

            MapObject mapObject = _map == null ? null : _map.instance;
            Map map = mapObject == null ? null : mapObject.instance;
            if (map == null)
                return false;

            Vector2 scale = mapObject._scale;
            float radius = Mathf.Abs(
                Vector2.Dot(
                    new Vector2((_width - 0.5f) * scale.x * Mathf.Sign(direction.x), (_height - 0.5f) * scale.y * Mathf.Sign(direction.z)), direction));

            Vector3 result = position + direction * (maxDistance + radius);
            int destination = _map.Get(result, __areaIndex);
            if (destination == __index)
                result -= direction * radius;
            else
            {
                Vector3 temp = -direction;
                float distance, minDistance = 0;
                int size = mapObject._width * mapObject._height;
                while (destination < 0 || destination >= size || (destination != __index && !map.Check(destination)))
                {
                    destination = _map.Get(result, ref temp, out distance);
                    minDistance += distance;
                    if (minDistance > maxDistance || distance < epsilon)
                    {
                        if (__instance.Set(map, __index))
                        {
                            bool isMove = false;
                            if (!Mathf.Approximately(direction.x, 0.0f) && !Mathf.Approximately(direction.z, 0.0f))
                            {
                                isMove = __Move(
                                    ref position, 
                                    new Vector3(Mathf.Sign(direction.x), 0.0f, 0.0f), 
                                    Mathf.Abs(maxDistance * Vector3.Dot(Vector3.right, direction)), 
                                    epsilon) || isMove;
                                isMove = __Move(
                                    ref position, 
                                    new Vector3(0.0f, 0.0f, Mathf.Sign(direction.z)), 
                                    Mathf.Abs(maxDistance * Vector3.Dot(Vector3.forward, direction)), 
                                    epsilon) || isMove;
                            }

                            return isMove;
                        }
                        else
                            break;
                    }

                    result += temp * distance;
                }

                int source = _map.Get(result, __areaIndex);
                if(source > destination)
                {
                    int width = mapObject.width;
                    if (source % width > destination % width)
                        result.x -= epsilon;
                    
                    if (source / width > destination / width)
                        result.z -= epsilon;
                }

                result -= direction * radius;
                destination = _map.Get(result, __areaIndex);
                if (destination != __index && __instance.Set(map, destination))
                    __index = destination;

                if (minDistance > 0.0f && !Mathf.Approximately(direction.x, 0.0f) && !Mathf.Approximately(direction.z, 0.0f))
                {
                    __Move(
                        ref result, 
                        new Vector3(
                            Mathf.Sign(direction.x), 0.0f, 0.0f), 
                        minDistance * Mathf.Abs(Vector3.Dot(Vector3.right, direction)), 
                        epsilon);
                    __Move(
                        ref result, 
                        new Vector3(0.0f, 0.0f, Mathf.Sign(direction.z)), 
                        minDistance * Mathf.Abs(Vector3.Dot(Vector3.forward, direction)), 
                        epsilon);
                }
            }

            position = result;

            return true;
        }
    }
}