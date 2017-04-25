using System.Collections.Generic;
using UnityEngine;

namespace ZG.RTS
{
    public abstract class ManagerBehaviour : MonoBehaviour
    {
        public interface IHandler
        {
            void Run(ObjectBehaviour target, int time, int elapsedTime);
        }

        private struct Handler
        {
            public int elapsedTime;
            public int time;
            public ObjectBehaviour target;
            public IHandler instance;
        }

        private struct Object
        {
            public ObjectBehaviour instance;
            public int time;

            public Object(ObjectBehaviour instance, int time)
            {
                this.instance = instance;
                this.time = time;
            }
        }
        
        public int time;

        [SerializeField]
        internal int _capacity;

        private Manager __instance;
        private LinkedList<Handler> __handlers;
        private int __realtime;
        private int __runTime;

        public int realTime
        {
            get
            {
                return __realtime;
            }
        }

        public int runTime
        {
            get
            {
                return __runTime;
            }
        }

        public Manager instance
        {
            get
            {
                return __instance;
            }
        }
        
        public abstract IHandler Create();

        public abstract IHandler Create(InfoMove move);

        public abstract IHandler Create(InfoSet set);

        public abstract IHandler Create(InfoHand hand);
        
        public void Awake()
        {
            __instance = new Manager(_capacity);
        }
        
        public void FixedUpdate()
        {
            int deltaTime = Mathf.RoundToInt(Time.fixedDeltaTime * 1000.0f);
            __realtime += deltaTime;
            while (__realtime > __runTime)
            {
                __Set(__instance.Run(time), __runTime);

                __runTime += time;
            }

            Handler handler;
            for (LinkedListNode<Handler> node = __handlers == null ? null : __handlers.First; node != null; node = node.Next)
            {
                handler = node.Value;
                if (handler.elapsedTime > __realtime)
                    break;
                else
                {
                    if (handler.instance != null)
                        handler.instance.Run(handler.target, handler.time, Mathf.Min(handler.time, __realtime - handler.elapsedTime));

                    if ((handler.elapsedTime + handler.time) <= __realtime)
                        __handlers.Remove(node);
                }
            }
        }

        public void Set(ByteArray<Info> infos)
        {
            __Set(infos, __realtime);
        }

        private void __Set(Handler handler)
        {
            if (__handlers == null)
                __handlers = new LinkedList<Handler>();

            LinkedListNode<Handler> node = __handlers.First;
            while(node != null)
            {
                if (node.Value.elapsedTime > handler.elapsedTime)
                    break;

                node = node.Next;
            }

            if (node == null)
                __handlers.AddLast(handler);
            else
                __handlers.AddBefore(node, handler);
        }

        private void __Set(ByteArray<Info> infos, int time)
        {
            Handler handler = new Handler();
            ObjectBehaviour.Instance instance;
            foreach (Info info in infos)
            {
                handler.elapsedTime = info.elapsedTime + time;
                handler.time = info.time;

                instance = info.instance as ObjectBehaviour.Instance;
                handler.target = instance == null ? null : instance.parent;
                switch (info.type)
                {
                    case Info.Type.Delay:
                        handler.instance = Create();
                        break;
                    case Info.Type.Move:
                        handler.instance = Create(info.move);
                        break;
                    case Info.Type.Set:
                        handler.instance = Create(info.set);
                        break;
                    case Info.Type.Hand:
                        handler.instance = Create(info.hand);
                        break;
                }

                __Set(handler);
            }
        }
    }
}