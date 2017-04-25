using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace ZG.RTS
{
    public class Manager
    {
        private IntPtr __instance;

#if DEBUG
        private int __index;

        private static int __count = 0;

        private const string __NAME = "pManager";
        public string name
        {
            get
            {
                return __NAME + __index;
            }
        }
#endif

        internal Pool<Object> _objects;

        internal IntPtr instance
        {
            get
            {
                return __instance;
            }
        }


        /// <summary>
        /// 构建
        /// </summary>
        /// <param name="width">
        /// 宽
        /// </param>
        /// <param name="height">
        /// 高
        /// </param>
        /// <param name="isOblique">
        /// 是否可以斜着走
        /// </param>
        public Manager(int capacity)
        {

#if DEBUG
            __index = __count++;
            Lib.LogVar("LPZGTILEMANAGER", name);
            Lib.LogCall(name, "ZGRTSCreateManager", (uint)capacity);
#endif

            __instance = Lib.ZGRTSCreateManager((uint)capacity);
        }

        ~Manager()
        {

#if DEBUG
            Lib.LogCall(null, "ZGRTSDestroy", name);
#endif

            Lib.ZGRTSDestroy(__instance);
        }
        
        public ByteArray<Info> Run(int time)
        {
#if DEBUG
            Lib.LogVar("ZGUINT", "uInfoCount");
            Lib.LogCall(null,
                "ZGRTSRun",
                name,
                (uint)time,
                "&uInfoCount");
#endif

            uint infoCount = 0;
            IntPtr infos = Lib.ZGRTSRun(
                __instance,
                (uint)time,
                out infoCount);
            
            return new ByteArray<Info>(infos, (int)infoCount);
        }
    }
}