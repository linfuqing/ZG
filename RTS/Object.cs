using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace ZG.RTS
{
    /// <summary>
    /// 地图对象，指作战单位。
    /// </summary>
    public class Object
    {
        private enum AttributeType
        {
            MoveTime, 
            SetTime, 
            HandTime,
            BreakTime,
            HP,
            Attack,
            Defense = Attack + ELEMENT_COUNT
        }

        private static Dictionary<IntPtr, Object> __objects = null;

        public const int ELEMENT_COUNT = 2;
        
        private IntPtr __instance;
        private Action __action;
        private Map __map;
        private Manager __manager;

#if DEBUG
        private int __index;
            
        private static int __count = 0;
        private const string __NAME = "pObject";

        public string name
        {
            get
            {
                return __NAME + __index;
            }
        }
#endif

        /// <summary>
        /// 阵营，阵营相同为友方，不同为敌对。
        /// </summary>
        public int camp
        {
            set
            {
#if DEBUG
                Lib.LogCall(null, "ZGRTSSetCampToObject", name, (uint)value);
#endif

                Lib.ZGRTSSetCampToObject(__instance, (uint)value);
            }
        }
        
        public int label
        {
            get
            {
#if DEBUG
                Lib.LogCall(null, "ZGRTSGetLabelFromObject", name);
#endif

                return (int)Lib.ZGRTSGetLabelFromObject(__instance);
            }

            set
            {
#if DEBUG
                Lib.LogCall(null, "ZGRTSSetLabelToObject", name, (uint)value);
#endif

                Lib.ZGRTSSetLabelToObject(__instance, (uint)value);
            }
        }

        public int moveTime
        {
            get
            {

#if DEBUG
                Lib.LogCall(null, "ZGRTSGetAttributeFromObject", name, (uint)AttributeType.MoveTime);
#endif

                return (int)Lib.ZGRTSGetAttributeFromObject(__instance, (uint)(AttributeType.MoveTime));
            }

            set
            {
#if DEBUG
                Lib.LogCall(null, "ZGRTSSetAttributeToObject", name, (uint)AttributeType.MoveTime, (uint)value);
#endif

                Lib.ZGRTSSetAttributeToObject(__instance, (uint)(AttributeType.MoveTime), (uint)value);
            }
        }
        
        public int setTime
        {
            get
            {

#if DEBUG
                Lib.LogCall(null, "ZGRTSGetAttributeFromObject", name, (uint)AttributeType.SetTime);
#endif

                return (int)Lib.ZGRTSGetAttributeFromObject(__instance, (uint)(AttributeType.SetTime));
            }

            set
            {
#if DEBUG
                Lib.LogCall(null, "ZGRTSSetAttributeToObject", name, (uint)AttributeType.SetTime, (uint)value);
#endif

                Lib.ZGRTSSetAttributeToObject(__instance, (uint)(AttributeType.SetTime), (uint)value);
            }
        }


        public int handTime
        {
            get
            {

#if DEBUG
                Lib.LogCall(null, "ZGRTSGetAttributeFromObject", name, (uint)AttributeType.HandTime);
#endif

                return (int)Lib.ZGRTSGetAttributeFromObject(__instance, (uint)(AttributeType.HandTime));
            }

            set
            {
#if DEBUG
                Lib.LogCall(null, "ZGRTSSetAttributeToObject", name, (uint)AttributeType.HandTime, (uint)value);
#endif

                Lib.ZGRTSSetAttributeToObject(__instance, (uint)(AttributeType.HandTime), (uint)value);
            }
        }

        /// <summary>
        /// HP。
        /// </summary>
        public int hp
        {
            get
            {
#if DEBUG
                Lib.LogCall(null, "ZGRTSGetAttributeFromObject", name, (uint)AttributeType.HP);
#endif

                return (int)Lib.ZGRTSGetAttributeFromObject(__instance, (uint)(AttributeType.HP));
            }

            set
            {

#if DEBUG
                Lib.LogCall(null, "ZGRTSSetAttributeToObject", name, (uint)AttributeType.HP, (uint)value);
#endif

                Lib.ZGRTSSetAttributeToObject(__instance, (uint)(AttributeType.HP), (uint)value);
            }
        }

        /// <summary>
        /// 行动力。
        /// </summary>
        public int distance
        {
            set
            {

#if DEBUG
                Lib.LogCall(null, "ZGRTSSetDistanceToObject", name, (uint)value);
#endif

                Lib.ZGRTSSetDistanceToObject(__instance, (uint)value);
            }
        }

        /// <summary>
        /// 最大行进格子数。
        /// </summary>
        public int range
        {
            set
            {

#if DEBUG
                Lib.LogCall(null, "ZGRTSSetRangeToObject", name, (uint)value);
#endif

                Lib.ZGRTSSetRangeToObject(__instance, (uint)value);
            }
        }

        /// <summary>
        /// 所处的地图坐标索引。
        /// </summary>
        public int index
        {
            get
            {

#if DEBUG
                Lib.LogCall(null, "ZGRTSGetIndexFromObject", name);
#endif

                return (int)Lib.ZGRTSGetIndexFromObject(__instance);
            }
        }

        public Action action
        {
            get
            {
                return __action;
            }

            set
            {

#if DEBUG
                Lib.LogCall(null, "ZGRTSSetActionToObject", name, value == null ? IntPtr.Zero.ToString() : value.name);
#endif

                if (Lib.ZGRTSSetActionToObject(__instance, value == null ? IntPtr.Zero : value.instance) == 0)
                    return;

                __action = value;
            }
        }

        public Map map
        {
            get
            {
                return __map;
            }
        }
        
        public Manager manager
        {
            get
            {
                return __manager;
            }

            set
            {

                if (__manager != null)
                {

#if DEBUG
                    Lib.LogCall(null, "ZGRTSRemoveObjectFromManager", name, __manager.name);
#endif

                    if (Lib.ZGRTSRemoveObjectFromManager(__instance, __manager.instance) == 0)
                        return;

                    __manager = null;
                }

                if (value != null)
                {
#if DEBUG
                    Lib.LogCall(null, "ZGRTSAddObjectToManager", name, value.name);
#endif

                    if (Lib.ZGRTSAddObjectToManager(__instance, value.instance) == 0)
                        return;
                }

                __manager = value;
            }
        }

        internal IntPtr instance
        {
            get
            {
                return __instance;
            }
        }

        /// <summary>
        /// 构造。
        /// </summary>
        /// <param name="width">
        /// 填1
        /// </param>
        /// <param name="height">
        /// 填1
        /// </param>
        /// <param name="offset">
        /// 填0
        /// </param>
        public Object(
            int width,
            int height,
            int offset)
        {
#if DEBUG
            __index = __count++;
            Lib.LogVar("LPZGTILEMANAGEROBJECT", name);
            Lib.LogCall(name, 
                "ZGRTSCreateObject", 
                (uint)width,
                (uint)height,
                (uint)offset);
#endif
            __instance = Lib.ZGRTSCreateObject(
                (uint)width,
                (uint)height,
                (uint)offset);

            if (__objects == null)
                __objects = new Dictionary<IntPtr, Object>();

            __objects.Add(__instance, this);
        }

        ~Object()
        {

#if DEBUG
            Lib.LogCall(null, "ZGRTSDestroy", name);
#endif

            Lib.ZGRTSDestroy(__instance);

            if (__objects != null)
                __objects.Remove(__instance);
        }

        public bool Unset()
        {
#if DEBUG
            Lib.LogCall(null, "ZGRTSUnsetObjectFromMap", name);
#endif

            if (Lib.ZGRTSUnsetObjectFromMap(__instance) == 0)
                return false;

            __map = null;

            return true;
        }
        
        public bool Set(Map map, int index)
        {
            if (map == null)
                return false;

            Unset();

#if DEBUG
            Lib.LogCall(null, "ZGRTSSetObjectToMap", name, map.name, (uint)index);
#endif

            if (Lib.ZGRTSSetObjectToMap(__instance, map.instance, (uint)index) == 0)
                return false;

            __map = map;

            return true;
        }

        /// <summary>
        /// 设置对象的攻击值。
        /// </summary>
        /// <param name="index">
        /// 攻击的索引，0为物攻，1为魔攻。
        /// </param>
        /// <param name="value">
        /// 攻击值。
        /// </param>
        /// <returns>
        /// 成功则返回<see cref="true"/>，否则为<see cref="false"/>。
        /// </returns>
        public bool SetAttack(int index, int value)
        {
            if (index < 0 || index >= ELEMENT_COUNT)
                return false;

#if DEBUG
            Lib.LogCall(null, "ZGRTSSetAttributeToObject", name, (uint)((int)AttributeType.Attack + index), (uint)value);
#endif

            Lib.ZGRTSSetAttributeToObject(__instance, (uint)((int)(AttributeType.Attack) + index), (uint)value);

            return true;
        }

        /// <summary>
        /// 设置对象的防御值。
        /// </summary>
        /// <param name="index">
        /// 攻击的索引，0为物防，1为魔防。
        /// </param>
        /// <param name="value">
        /// 防御值。
        /// </param>
        /// <returns>
        /// 成功则返回<see cref="true"/>，否则为<see cref="false"/>。
        /// </returns>
        public bool SetDefense(int index, int value)
        {
            if (index < 0 || index >= ELEMENT_COUNT)
                return false;

#if DEBUG
            Lib.LogCall(null, "ZGRTSSetAttributeToObject", name, (uint)((int)AttributeType.Defense + index), (uint)value);
#endif

            Lib.ZGRTSSetAttributeToObject(__instance, (uint)((int)(AttributeType.Defense) + index), (uint)value);

            return true;
        }
        
        public bool Do(int time, int index, Manager manager, out ByteArray<Info> infos)
        {
            if (manager == null)
            {
                infos = default(ByteArray<Info>);

                return false;
            }

#if DEBUG
            Lib.LogVar("ZGUINT", "uInfoCount");
            Lib.LogVar("LPZGRTSINFO", "pInfos");
            Lib.LogCall(null,
                "ZGRTSDo",
                manager.name,
                name,
                (uint)index,
                (uint)time, 
                "&uInfoCount",
                "&pInfos");
#endif

            uint infoCount = 0;
            IntPtr outInfos = IntPtr.Zero;
            int result = (int)Lib.ZGRTSDo(
                manager.instance,
                __instance,
                (uint)index,
                (uint)time,
                out infoCount,
                out outInfos);

            infos = new ByteArray<Info>(outInfos, (int)infoCount);

            return result != 0;
        }

        public void Break(int time)
        {
#if DEBUG
            Lib.LogCall(null,
                "ZGRTSBreak",
                name,
                (uint)time);
#endif

            Lib.ZGRTSBreak(__instance, (uint)time);
        }

        internal static Object Get(IntPtr instance)
        {
            if (__objects == null)
                return null;

            Object result;
            if (__objects.TryGetValue(instance, out result))
                return result;

            return null;
        }
    }
}