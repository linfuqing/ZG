using UnityEngine;

namespace ZG.RTS
{
    public class ActionNormalObject : ActionObject
    {

        [SerializeField]
        internal int _range = 10;

        protected override Action _Create(int numChildren)
        {
            ActionNormal actionNormal = new ActionNormal(numChildren);
            actionNormal.range = _range;

            return actionNormal;
        }
    }
}