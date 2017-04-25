using System;
using UnityEngine;

namespace ZG.RTS
{
    public class MapBehaviour : MonoBehaviour
    {
        private struct Level
        {
            public int areaIndex;
            public int levelIndex;

            public Level(int areaIndex, int levelIndex)
            {
                this.areaIndex = areaIndex;
                this.levelIndex = levelIndex;
            }
        }

        [Serializable]
        public struct Area
        {
            public float offset;
            public float scale;

            public int[] levels;
        }

        public MapObject instance;

        public Area[] areas;
        private Pool<Level> __levels;

        public void Awake()
        {
            int numAreas = areas == null ? 0 : areas.Length;
            if(numAreas > 0)
            {
                int numLevels, j;
                Area area;
                for(int i = 0; i < numAreas; ++i)
                {
                    area = areas[i];
                    numLevels = area.levels == null ? 0 : area.levels.Length;

                    for(j = 0; j < numLevels; ++j)
                    {
                        if (__levels == null)
                            __levels = new Pool<Level>();

                        __levels.Insert(area.levels[j], new Level(i, j));
                    }
                }
            }
        }

        public void OnDrawGizmos()
        {
            if (instance == null)
                return;

            Gizmos.color = Color.black;

            Vector3 position = transform.position;
            float halfWidth = instance._width * instance._scale.x * 0.5f, halfHeight = instance._height * instance._scale.y * 0.5f, temp = -halfWidth;
            for (int i = 0; i < instance._width; ++i)
            {
                Gizmos.DrawLine(position + new Vector3(temp, 0.0f, -halfHeight), position + new Vector3(temp, 0.0f, halfHeight));
                temp += instance._scale.x;
            }

            Gizmos.DrawLine(position + new Vector3(temp, 0.0f, -halfHeight), position + new Vector3(temp, 0.0f, halfHeight));

            temp = -halfHeight;
            for (int i = 0; i < instance._height; ++i)
            {
                Gizmos.DrawLine(position + new Vector3(-halfWidth, 0.0f, temp), position + new Vector3(halfWidth, 0.0f, temp));
                temp += instance._scale.y;
            }

            Gizmos.DrawLine(position + new Vector3(-halfWidth, 0.0f, temp), position + new Vector3(halfWidth, 0.0f, temp));
            
            Map map = instance.instance;
            if (map != null)
            {
                Gizmos.color = Color.red;

                Vector3 source, destination;
                for (int i = 0; i < instance._width; ++i)
                {
                    for (int j = 0; j < instance._height; ++j)
                    {
                        if (!map.Check(i + j * instance._width))
                        {
                            source = position + new Vector3(i * instance._scale.x - halfWidth, 0.0f, j * instance._scale.y - halfHeight);
                            destination = source + new Vector3(instance._scale.x, 0.0f, instance._scale.y);
                            Gizmos.DrawLine(source, destination);
                            //GL.Vertex3(source.x, source.y, source.z);
                            //GL.Vertex3(destination.x, destination.y, destination.z);
                            source.z += instance._scale.y;
                            destination.z -= instance._scale.y;
                            //GL.Vertex3(source.x, source.y, source.z);
                            //GL.Vertex3(destination.x, destination.y, destination.z);
                            Gizmos.DrawLine(source, destination);
                        }
                    }
                }


                //GL.End();
            }
        }
        
        public int Get(Vector3 position, int areaIndex)
        {
            if (instance == null)
                return -1;

            Vector3 temp = position;
            temp -= transform.position;

            temp.x += instance._width * instance._scale.x * 0.5f;
            temp.x /= instance._scale.x;
            int x = Mathf.FloorToInt(temp.x);
            if (x < 0 || x >= instance._width)
                return -1;

            temp.z += instance._height * instance._scale.y * 0.5f;
            temp.z /= instance._scale.y;
            int z = Mathf.FloorToInt(temp.z);
            if (z < 0 || z >= instance._height)
                return -1;

            int mapIndex = x + z * instance._width;

            int numAreas = areas == null ? 0 : areas.Length;
            if(areaIndex >= 0 && areaIndex < numAreas)
            {
                Area area = areas[areaIndex];
                temp.y -= area.offset;
                temp.y /= area.scale;
                int y = Mathf.FloorToInt(temp.y), numLevels = area.levels == null ? 0 : area.levels.Length;
                if (y >= 0 && y < numLevels)
                    return mapIndex + area.levels[y] * instance._width * instance._height;
            }

            return mapIndex;
        }

        public Vector3 Get(int index)
        {
            if (instance == null)
                return Vector3.zero;

            int size = instance._width * instance._height,
                levelIndex = index / size, 
                offset = levelIndex * size, 
                mapIndex = index - offset;
            
            float y = 0.0f;
            if(__levels != null)
            {
                Level level;
                if(__levels.TryGetValue(levelIndex, out level) && level.areaIndex >= 0 && level.areaIndex < (areas == null ? 0 : areas.Length))
                {
                    Area area = areas[level.areaIndex];
                    y = level.levelIndex * area.scale;
                    y += area.offset;
                }
            }

            return transform.position + new Vector3(
                (mapIndex % instance._width + 0.5f) * instance._scale.x - instance._width * instance._scale.x * 0.5f, 
                y, 
                (mapIndex / instance._width + 0.5f) * instance._scale.y - instance._height * instance._scale.y * 0.5f);
        }

        public int Get(Vector3 position, ref Vector3 direction, out float distance)
        {
            if (instance == null)
            {
                distance = 0.0f;

                return -1;
            }

            position -= transform.position;
            position.x += instance._width * instance._scale.x * 0.5f;
            position.z += instance._height * instance._scale.y * 0.5f;

            return __Get(
                Mathf.FloorToInt(position.x / instance._scale.x),
                Mathf.FloorToInt(position.z / instance._scale.y),
                position,
                ref direction,
                out distance);
        }

        private int __Get(int sourceX, int sourceY, Vector3 position, ref Vector3 direction, out float distance)
        {
            if (instance == null)
            {
                distance = 0.0f;

                return -1;
            }

            if (sourceX < 0 || sourceX >= instance._width || sourceY < 0 || sourceY >= instance._height)
            {
                Vector3 temp = position;
                temp.x = Mathf.Clamp(temp.x, 0.0f, instance._width * instance._scale.x);
                temp.z = Mathf.Clamp(temp.z, 0.0f, instance._height * instance._scale.y);
                direction = temp - position;
                distance = direction.magnitude;
                if(distance > 0.0f)
                    direction /= distance;

                return Mathf.Clamp(sourceX, 0, instance._width - 1) + Mathf.Clamp(sourceY, 0, instance._height - 1) * instance._width;
            }

            int destinationX = sourceX + (direction.x > 0.0f ? 1 : 0),
                destinationY = sourceY + (direction.z > 0.0f ? 1 : 0);

            /*destinationX = Mathf.Clamp(destinationX, 0, _width);
            destinationY = Mathf.Clamp(destinationY, 0, _height);*/

            float distanceX = destinationX * instance._scale.x - position.x,
                distanceY = destinationY * instance._scale.y - position.z;

            if (Mathf.Approximately(direction.x, 0.0f))
            {
                distance = Mathf.Abs(distanceY);

                if (distance > 0.0f)
                    return Mathf.Clamp(sourceX, 0, instance._width - 1) + Mathf.Clamp(sourceY + (int)Mathf.Sign(direction.z), 0, instance._height - 1) * instance._width;

                return __Get(
                    sourceX,
                    sourceY + (int)Mathf.Sign(direction.z),
                    position,
                    ref direction,
                    out distance);
            }

            if (Mathf.Approximately(direction.z, 0.0f))
            {
                distance = Mathf.Abs(distanceX);

                if (distance > 0.0f)
                    return Mathf.Clamp(sourceX + (int)Mathf.Sign(direction.x), 0, instance._width - 1) + Mathf.Clamp(sourceY, 0, instance._height - 1) * instance._width;

                return __Get(
                    sourceX + (int)Mathf.Sign(direction.x),
                    sourceY,
                    position,
                    ref direction,
                    out distance);
            }

            distanceX /= direction.x;
            distanceY /= direction.z;

            if (Mathf.Approximately(distanceX, distanceY))
            {
                if (distanceX > 0.0f)
                {
                    distance = distanceX;

                    return Mathf.Clamp(sourceX + (int)Mathf.Sign(direction.x), 0, instance._width - 1) + Mathf.Clamp(sourceY + (int)Mathf.Sign(direction.z), 0, instance._height - 1) * instance._width;
                }

                return __Get(
                    sourceX + (int)Mathf.Sign(direction.x),
                    sourceY + (int)Mathf.Sign(direction.z),
                    position,
                    ref direction,
                    out distance);
            }

            if (distanceX < distanceY)
            {
                if (distanceX > 0.0f)
                {
                    distance = distanceX;

                    return Mathf.Clamp(sourceX + (int)Mathf.Sign(direction.x), 0, instance._width - 1) + Mathf.Clamp(sourceY, 0, instance._height - 1) * instance._width;
                }

                return __Get(
                    sourceX + (int)Mathf.Sign(direction.x),
                    sourceY,
                    position,
                    ref direction,
                    out distance);
            }

            if (distanceY > 0.0f)
            {
                distance = distanceY;

                return Mathf.Clamp(sourceX, 0, instance._width - 1) + Mathf.Clamp(sourceY + (int)Mathf.Sign(direction.z), 0, instance._height - 1) * instance._width;
            }

            return __Get(
                    sourceX,
                    sourceY + (int)Mathf.Sign(direction.z),
                    position,
                    ref direction,
                    out distance);
        }
    }
}