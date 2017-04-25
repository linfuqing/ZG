using System;

namespace ZG.RTS
{
    public class Map
    {

        private IntPtr __instance;

#if DEBUG
        private int __index;

        private static int __count = 0;

        private const string __NAME = "pMap";

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

        public Map(int width, int height, int depth, bool isOblique)
        {
#if DEBUG
            __index = __count++;
            Lib.LogVar("LPZGTILEMAP", name);
            Lib.LogCall(name, "ZGRTSCreateMap", (uint)width, (uint)height, (uint)depth, isOblique ? 1 : 0);
#endif

            __instance = Lib.ZGRTSCreateMap((uint)width, (uint)height, (uint)depth, isOblique ? 1 : 0);
        }

        ~Map()
        {
#if DEBUG
            Lib.LogCall(null, "ZGRTSDestroyMap", name);
#endif

            Lib.ZGRTSDestroyMap(__instance);
        }


        public bool Check(int index)
        {


#if DEBUG
            //Lib.LogCall(null, "ZGRTSGetMap", name, (uint)index);
#endif


            return Lib.ZGRTSGetMap(__instance, (uint)index) == 0;
        }

        /// <summary>
        /// 打开指定格子，让该格子可行走。
        /// </summary>
        /// <param name="index">
        /// 格子的索引。
        /// </param>
        public void Enable(int index)
        {

#if DEBUG
            Lib.LogCall(null, "ZGRTSSetMap", name, (uint)index, 0);
#endif

            Lib.ZGRTSSetMap(__instance, (uint)index, 0);
        }

        /// <summary>
        /// 关闭指定的格子，让该格子不可走。
        /// </summary>
        /// <param name="index">
        /// 格子的索引。
        /// </param>
        public void Disable(int index)
        {

#if DEBUG
            Lib.LogCall(null, "ZGRTSSetMap", name, (uint)index, 1);
#endif

            Lib.ZGRTSSetMap(__instance, (uint)index, 1);
        }

        /// <summary>
        /// 设置单个格子的属性。
        /// </summary>
        /// <param name="index">
        /// 格子所对应的索引。
        /// </param>
        /// <param name="distance">
        /// 对象经过该格子需消耗的距离点数。
        /// </param>
        public void Set(int index, int distance)
        {

#if DEBUG
            Lib.LogCall(null, "ZGRTSSetDistanceToMap", name, (uint)index, (uint)distance);
#endif

            Lib.ZGRTSSetDistanceToMap(__instance, (uint)index, (uint)distance);
        }
    }
}