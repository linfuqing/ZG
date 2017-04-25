using UnityEditor;

namespace ZG.RTS
{
    public static class EditorHelper
    {
        [MenuItem("Assets/Create/Action Active Object")]
        public static void CreateActionActive()
        {
            ZG.EditorHelper.CreateAsset<ActionActiveObject>("Action Active Object");
        }

        [MenuItem("Assets/Create/Action Normal Object")]
        public static void CreateActionNormal()
        {
            ZG.EditorHelper.CreateAsset<ActionNormalObject>("Action Normal Object");
        }
    }
}