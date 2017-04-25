using System;
using System.Runtime.InteropServices;

namespace ZG.RTS
{
    [StructLayout(LayoutKind.Sequential)]
    public struct InfoTarget
    {
        private IntPtr pTileObject;
        private uint uHP;


        /// <summary>
        /// 对象受影响后所获得的HP。（当前版本如果受到伤害后HP小于或等于0，对象将被移除）
        /// </summary>
        public int hp
        {
            get
            {
                return (int)uHP;
            }
        }

        /// <summary>
        /// 所影响的对象。
        /// </summary>
        /// <param name="map">
        /// 对象所在的地图。
        /// </param>
        /// <returns>
        /// 如果不存在，则为<see cref="null"/>。
        /// </returns>
        public Object instance
        {
            get
            {
                return Object.Get(pTileObject);
            }
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct InfoMove
    {
        private uint uFromIndex;
        private uint uToIndex;

        public int fromIndex
        {
            get
            {
                return (int)uFromIndex;
            }
        }

        public int toIndex
        {
            get
            {
                return (int)uToIndex;
            }
        }
    };

    [StructLayout(LayoutKind.Sequential)]
    public struct InfoSet
    {
        private uint uIndex;
        private IntPtr pTileObjectAction;

        public int index
        {
            get
            {
                return (int)uIndex;
            }
        }

        public Action action
        {
            get
            {
                return Action.Get(pTileObjectAction);
            }
        }
    };

    [StructLayout(LayoutKind.Sequential)]
    public struct InfoHand
    {
        private uint uIndex;
        private IntPtr pTileObjectAction;
        private IntPtr pTargets;
        private uint uTargetCount;

        public int index
        {
            get
            {
                return (int)uIndex;
            }
        }

        public Action action
        {
            get
            {
                return Action.Get(pTileObjectAction);
            }
        }

        public ByteArray<InfoTarget> targets
        {
            get
            {
                return new ByteArray<InfoTarget>(pTargets, (int)uTargetCount);
            }
        }
    };

    [StructLayout(LayoutKind.Sequential)]
    public struct Info
    {
        public enum Type
        {
            Delay,
            Move,
            Set,
            Hand
        }

        private Type eType;
        private uint uElapsedTime;
        private uint uTime;
        private IntPtr pTileManagerObject;
        private IntPtr __instance;

        public Type type
        {
            get
            {
                return eType;
            }
        }

        public int elapsedTime
        {
            get
            {
                return (int)uElapsedTime;
            }
        }

        public int time
        {
            get
            {
                return (int)uTime;
            }
        }

        public Object instance
        {
            get
            {
                return Object.Get(pTileManagerObject);
            }
        }

        public InfoMove move
        {
            get
            {
#if DEBUG
                if (eType != Type.Move)
                    throw new InvalidOperationException();
#endif

                return (InfoMove)Marshal.PtrToStructure(__instance, typeof(InfoMove));
            }
        }

        public InfoSet set
        {
            get
            {
#if DEBUG
                if (eType != Type.Set)
                    throw new InvalidOperationException();
#endif

                return (InfoSet)Marshal.PtrToStructure(__instance, typeof(InfoSet));
            }
        }

        public InfoHand hand
        {
            get
            {
#if DEBUG
                if (eType != Type.Hand)
                    throw new InvalidOperationException();
#endif

                return (InfoHand)Marshal.PtrToStructure(__instance, typeof(InfoHand));
            }
        }
    }

    internal static class Lib
    {
#if UNITY_EDITOR || UNITY_STANDALONE_WIN
        public const string LIB_NAME = "ZG";
#elif UNITY_STANDALONE_LINUX
        public const string LIB_NAME = "ZGLinux";
#elif UNITY_ANDROID
        public const string LIB_NAME = "ZGAndroid";
#endif

#if DEBUG
        private static System.Collections.Generic.HashSet<string> varNames;
        private static string __logPath = System.IO.Path.Combine(UnityEngine.Application.temporaryCachePath, "Code.txt");
        private static string __log;

        public static string log
        {
            get
            {
                return __log;
            }
        }
        
        public static void LogVar(string typeName, string varName)
        {
            if(varNames == null)
                varNames = new System.Collections.Generic.HashSet<string>();
            else if(varNames.Contains(varName))
                return;

            varNames.Add(varName);

            __Log(typeName + ' ' + varName + ";\n");

            Save();
        }
        
        public static void LogCall(string returnName, string methodName, params object[] paramters)
        {
            if (!string.IsNullOrEmpty(returnName))
                __Log(returnName + " = ");

            __Log(methodName + '(');
            int length = paramters == null ? 0 : paramters.Length;
            if (length > 0)
            {
                object parameter = paramters[0];
                if (parameter == null)
                    __Log("ZG_NULL");
                else
                    __Log(parameter.ToString());

                for (int i = 1; i < length; ++i)
                {
                    parameter = paramters[i];

                    if (parameter == null)
                        __Log(", ZG_NULL");
                    else
                        __Log(", " + parameter.ToString());
                }
            }
            __Log(");\n");

            Save();
        }

        private static void __Log(string message)
        {
            //__log += message;
        }

        private static void Save()
        {
            //System.IO.File.WriteAllText(__logPath, __log);
        }

        [UnityEngine.RuntimeInitializeOnLoadMethod]
        private static void __Init()
        {
            varNames = null;

            if (System.IO.File.Exists(__logPath))
                System.IO.File.Delete(__logPath);
        }
#endif
#if ZG_LIB_INVALID
        public static void ZGRTSDestroy(IntPtr pData) {}
        
        public static IntPtr ZGRTSCreateObject(
            uint uWidth,
            uint uHeight,
            uint uOffset) { return IntPtr.Zero; }
        
        public static IntPtr ZGRTSCreateActionNormal(
            uint uChildCount) { return IntPtr.Zero; }
        
        public static IntPtr ZGRTSCreateActionActive(
            uint uDistance,
            uint uRange,
            uint uChildCount) { return IntPtr.Zero; }
        
        public static IntPtr ZGRTSCreateMap(uint uWidth, uint uHeight, int bIsOblique) { return IntPtr.Zero; }
        
        public static IntPtr ZGRTSCreateManager(uint uCapacity) { return IntPtr.Zero; }
        
        public static IntPtr ZGRTSDestroyMap(IntPtr pTileMap) { return IntPtr.Zero; }
        
        public static int ZGRTSGetMap(IntPtr pTileMap, uint uIndex) { return 0; }
        
        public static int ZGRTSSetMap(IntPtr pTileMap, uint uIndex, int bValue) { return 0; }
        
        public static int ZGRTSSetObjectToMap(IntPtr pTileManagerObject, IntPtr pTileMap, uint uIndex) { return 0; }
        
        public static int ZGRTSUnsetObjectFromMap(IntPtr pTileManagerObject) { return 0; }
        
        public static int ZGRTSAddObjectToManager(IntPtr pTileManagerObject, IntPtr pTileManager) { return 0; }
        
        public static int ZGRTSRemoveObjectFromManager(IntPtr pTileManagerObject, IntPtr pTileManager) { return 0; }
        
        public static int ZGRTSDo(
            IntPtr pTileManager,
            IntPtr pTileManagerObject,
            uint uIndex,
            uint uTime, 
            out uint puInfoCount,
            out IntPtr ppInfos) { puInfoCount = 0; ppInfos = IntPtr.Zero; return 0; }
        
        public static IntPtr ZGRTSRun(
            IntPtr pTileManager, 
            uint uTime, 
            out uint puInfoCount) { puInfoCount = 0; return IntPtr.Zero; }
        
        public static uint ZGRTSGetCampToObject(IntPtr pTileManagerObject) { return 0; }
        
        public static uint ZGRTSGetLabelFromObject(IntPtr pTileManagerObject) { return 0; }
        
        public static uint ZGRTSGetAttributeFromObject(IntPtr pTileManagerObject, uint uAttribute) { return 0; }
        
        public static uint ZGRTSGetIndexFromObject(IntPtr pTileManagerObject) { return 0; }
        
        public static IntPtr ZGRTSGetMapNodeFromMap(IntPtr pTileMap, uint uIndex) { return IntPtr.Zero; }
        
        public static uint ZGRTSGetIndexFromMapNode(IntPtr pNode) { return 0; }
        
        public static IntPtr ZGRTSGetNextFromMapNode(IntPtr pNode) { return IntPtr.Zero; }
        
        public static int ZGRTSSetCampToObject(IntPtr pTileManagerObject, uint uCamp) { return 0; }
        
        public static int ZGRTSSetLabelToObject(IntPtr pTileManagerObject, uint uLabel) { return 0; }
        
        public static int ZGRTSSetAttributeToObject(IntPtr pTileManagerObject, uint uAttribute, uint uValue) { return 0; }
        
        public static int ZGRTSSetDistanceToObject(IntPtr pTileManagerObject, uint uDistance) { return 0; }
        
        public static int ZGRTSSetRangeToObject(IntPtr pTileManagerObject, uint uRange) { return 0; }
        
        public static int ZGRTSSetActionToObject(IntPtr pTileManagerObject, IntPtr pActions) { return 0; }
        
        public static int ZGRTSSetChildToAction(IntPtr pTileObjectAction, IntPtr pChild, uint uIndex) { return 0; }
        
        public static int ZGRTSSetEvaluationToActionActive(IntPtr pTileObjectAction, uint uEvaluation) { return 0; }
        
        public static int ZGRTSSetMinEvaluationToActionActive(IntPtr pTileObjectAction, uint uMinEvaluation) { return 0; }
        
        public static int ZGRTSSetMaxEvaluationToActionActive(IntPtr pTileObjectAction, uint uMaxEvaluation) { return 0; }
        
        public static int ZGRTSSetMaxDistanceToActionActive(IntPtr pTileObjectAction, uint uMaxDistance) { return 0; }
        
        public static int ZGRTSSetMaxDepthToActionActive(IntPtr pTileObjectAction, uint uMaxDepth) { return 0; }
        
        public static int ZGRTSSetSearchLabelToActionActive(IntPtr pTileObjectAction, uint uSearchLabel) { return 0; }
        
        public static int ZGRTSSetSetLabelToActionActive(IntPtr pTileObjectAction, uint uSetLabel) { return 0; }
        
        public static int ZGRTSSetRangeToActionNormal(IntPtr pTileObjectAction, uint uRange) { return 0; }
        
        public static void ZGRTSSetDistanceToMap(IntPtr pTileMap, uint uIndex, uint uDistance) {}
#else
        [DllImport(LIB_NAME)]
        public static extern void ZGRTSDestroy(IntPtr pData);

        [DllImport(LIB_NAME)]
        public static extern IntPtr ZGRTSCreateObject(
            uint uWidth,
            uint uHeight,
            uint uOffset);

        [DllImport(LIB_NAME)]
        public static extern IntPtr ZGRTSCreateActionNormal(
            uint uChildCount);

        [DllImport(LIB_NAME)]
        public static extern IntPtr ZGRTSCreateActionActive(
            uint uDistance,
            uint uRange,
            uint uChildCount);

        [DllImport(LIB_NAME)]
        public static extern IntPtr ZGRTSCreateMap(uint uWidth, uint uHeight, uint uDepth, int bIsOblique);

        [DllImport(LIB_NAME)]
        public static extern IntPtr ZGRTSCreateManager(uint uCapacity);

        [DllImport(LIB_NAME)]
        public static extern IntPtr ZGRTSDestroyMap(IntPtr pTileMap);

        [DllImport(LIB_NAME)]
        public static extern int ZGRTSGetMap(IntPtr pTileMap, uint uIndex);

        [DllImport(LIB_NAME)]
        public static extern int ZGRTSSetMap(IntPtr pTileMap, uint uIndex, int bValue);

        [DllImport(LIB_NAME)]
        public static extern int ZGRTSSetObjectToMap(IntPtr pTileManagerObject, IntPtr pTileMap, uint uIndex);
        
        [DllImport(LIB_NAME)]
        public static extern int ZGRTSUnsetObjectFromMap(IntPtr pTileManagerObject);

        [DllImport(LIB_NAME)]
        public static extern int ZGRTSAddObjectToManager(IntPtr pTileManagerObject, IntPtr pTileManager);

        [DllImport(LIB_NAME)]
        public static extern int ZGRTSRemoveObjectFromManager(IntPtr pTileManagerObject, IntPtr pTileManager);

        [DllImport(LIB_NAME)]
        public static extern int ZGRTSDo(
            IntPtr pTileManager,
            IntPtr pTileManagerObject,
            uint uIndex,
            uint uTime, 
            out uint puInfoCount,
            out IntPtr ppInfos);

        [DllImport(LIB_NAME)]
        public static extern void ZGRTSBreak(IntPtr pTileManagerObject, uint uTime);

        [DllImport(LIB_NAME)]
        public static extern IntPtr ZGRTSRun(
            IntPtr pTileManager, 
            uint uTime, 
            out uint puInfoCount);

        [DllImport(LIB_NAME)]
        public static extern uint ZGRTSGetCampToObject(IntPtr pTileManagerObject);

        [DllImport(LIB_NAME)]
        public static extern uint ZGRTSGetLabelFromObject(IntPtr pTileManagerObject);

        [DllImport(LIB_NAME)]
        public static extern uint ZGRTSGetAttributeFromObject(IntPtr pTileManagerObject, uint uAttribute);

        [DllImport(LIB_NAME)]
        public static extern uint ZGRTSGetIndexFromObject(IntPtr pTileManagerObject);
        
        [DllImport(LIB_NAME)]
        public static extern IntPtr ZGRTSGetMapNodeFromMap(IntPtr pTileMap, uint uIndex);
        
        [DllImport(LIB_NAME)]
        public static extern uint ZGRTSGetIndexFromMapNode(IntPtr pNode);

        [DllImport(LIB_NAME)]
        public static extern IntPtr ZGRTSGetNextFromMapNode(IntPtr pNode);

        [DllImport(LIB_NAME)]
        public static extern int ZGRTSSetCampToObject(IntPtr pTileManagerObject, uint uCamp);

        [DllImport(LIB_NAME)]
        public static extern int ZGRTSSetLabelToObject(IntPtr pTileManagerObject, uint uLabel);

        [DllImport(LIB_NAME)]
        public static extern int ZGRTSSetAttributeToObject(IntPtr pTileManagerObject, uint uAttribute, uint uValue);

        [DllImport(LIB_NAME)]
        public static extern int ZGRTSSetDistanceToObject(IntPtr pTileManagerObject, uint uDistance);

        [DllImport(LIB_NAME)]
        public static extern int ZGRTSSetRangeToObject(IntPtr pTileManagerObject, uint uRange);
        
        [DllImport(LIB_NAME)]
        public static extern int ZGRTSSetActionToObject(IntPtr pTileManagerObject, IntPtr pActions);

        [DllImport(LIB_NAME)]
        public static extern int ZGRTSSetChildToAction(IntPtr pTileObjectAction, IntPtr pChild, uint uIndex);

        [DllImport(LIB_NAME)]
        public static extern int ZGRTSSetEvaluationToActionActive(IntPtr pTileObjectAction, uint uEvaluation);

        [DllImport(LIB_NAME)]
        public static extern int ZGRTSSetMinEvaluationToActionActive(IntPtr pTileObjectAction, uint uMinEvaluation);

        [DllImport(LIB_NAME)]
        public static extern int ZGRTSSetMaxEvaluationToActionActive(IntPtr pTileObjectAction, uint uMaxEvaluation);

        [DllImport(LIB_NAME)]
        public static extern int ZGRTSSetMaxDistanceToActionActive(IntPtr pTileObjectAction, uint uMaxDistance);

        [DllImport(LIB_NAME)]
        public static extern int ZGRTSSetMaxDepthToActionActive(IntPtr pTileObjectAction, uint uMaxDepth);

        [DllImport(LIB_NAME)]
        public static extern int ZGRTSSetSearchLabelToActionActive(IntPtr pTileObjectAction, uint uSearchLabel);
        
        [DllImport(LIB_NAME)]
        public static extern int ZGRTSSetSetLabelToActionActive(IntPtr pTileObjectAction, uint uSetLabel);

        [DllImport(LIB_NAME)]
        public static extern int ZGRTSSetRangeToActionNormal(IntPtr pTileObjectAction, uint uRange);

        [DllImport(LIB_NAME)]
        public static extern void ZGRTSSetDistanceToMap(IntPtr pTileMap, uint uIndex, uint uDistance);
#endif
    }
}