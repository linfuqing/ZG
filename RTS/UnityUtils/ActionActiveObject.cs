using UnityEngine;

namespace ZG.RTS
{
    public class ActionActiveObject : ActionObject
    {
        [SerializeField]
        internal int _distance = 1;
        [SerializeField]
        internal int _range = 0;
        [SerializeField]
        internal int _evaluation = 10;
        [SerializeField]
        internal int _minEvaluation = 10;
        [SerializeField]
        internal int _maxEvaluation = 100;
        [SerializeField]
        internal int _maxDistance = 100;
        [SerializeField]
        internal int _maxDepth = 100;
        [SerializeField]
        internal int _searchLabel = 1;
        [SerializeField]
        internal int _setLabel = 1;

        protected override Action _Create(int numChildren)
        {
            ActionActive actionActive = new ActionActive(_distance, _range, numChildren);
            actionActive.evaluation = _evaluation;
            actionActive.minEvaluation = _minEvaluation;
            actionActive.maxEvaluation = _maxEvaluation;
            actionActive.maxDistance = _maxDistance;
            actionActive.maxDepth = _maxDepth;
            actionActive.searchLabel = _searchLabel;
            actionActive.setLabel = _setLabel;

            return actionActive;
        }
    }
}