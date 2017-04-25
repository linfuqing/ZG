namespace ZG.RTS
{
    public class ActionNormal : Action
    {

        public int range
        {
            set
            {

#if DEBUG
                Lib.LogCall(null, "ZGRTSSetRangeToActionNormal", name, (uint)value);
#endif

                Lib.ZGRTSSetRangeToActionNormal(instance, (uint)value);
            }
        }

        public ActionNormal(int childCount) : base(
            Lib.ZGRTSCreateActionNormal((uint)childCount),
            childCount)
        {

#if DEBUG
            Lib.LogCall(name, "ZGRTSCreateActionNormal", (uint)childCount);
#endif
        }
    }
}