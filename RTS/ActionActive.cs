using System;
using System.Collections.Generic;

namespace ZG.RTS
{
    public class ActionActive : Action
    {
        
        public int evaluation
        {
            set
            {

#if DEBUG
                Lib.LogCall(null, "ZGRTSSetEvaluationToActionActive", name, (uint)value);
#endif

                Lib.ZGRTSSetEvaluationToActionActive(instance, (uint)value);

            }
        }

        public int minEvaluation
        {
            set
            {

#if DEBUG
                Lib.LogCall(null, "ZGRTSSetMinEvaluationToActionActive", name, (uint)value);
#endif

                Lib.ZGRTSSetMinEvaluationToActionActive(instance, (uint)value);
            }
        }

        public int maxEvaluation
        {
            set
            {

#if DEBUG
                Lib.LogCall(null, "ZGRTSSetMaxEvaluationToActionActive", name, (uint)value);
#endif

                Lib.ZGRTSSetMaxEvaluationToActionActive(instance, (uint)value);
            }
        }

        public int maxDistance
        {
            set
            {

#if DEBUG
                Lib.LogCall(null, "ZGRTSSetMaxDistanceToActionActive", name, (uint)value);
#endif

                Lib.ZGRTSSetMaxDistanceToActionActive(instance, (uint)value);
            }
        }

        public int maxDepth
        {
            set
            {

#if DEBUG
                Lib.LogCall(null, "ZGRTSSetMaxDepthToActionActive", name, (uint)value);
#endif

                Lib.ZGRTSSetMaxDepthToActionActive(instance, (uint)value);
            }
        }

        public int searchLabel
        {
            set
            {

#if DEBUG
                Lib.LogCall(null, "ZGRTSSetSearchLabelToActionActive", name, (uint)value);
#endif

                Lib.ZGRTSSetSearchLabelToActionActive(instance, (uint)value);
            }
        }

        public int setLabel
        {
            set
            {

#if DEBUG
                Lib.LogCall(null, "ZGRTSSetSetLabelToActionActive", name, (uint)value);
#endif

                Lib.ZGRTSSetSetLabelToActionActive(instance, (uint)value);
            }
        }

        /// <summary>
        /// 构造
        /// </summary>
        /// <param name="distance">
        /// 攻击距离。
        /// </param>
        /// <param name="range">
        /// 攻击范围。
        /// </param>
        public ActionActive(int distance, int range, int childCount) : base(
            Lib.ZGRTSCreateActionActive((uint)distance, (uint)range, (uint)childCount), 
            childCount)
        {

#if DEBUG
            Lib.LogCall(name, "ZGRTSCreateActionActive", (uint)distance, (uint)range, (uint)childCount);
#endif
        }
    }
}