public class AdaUtility {

    public static void deepCopyArrays(Object dst, Object src) {
        if (dst == null || src == null) return;

        Class dstClass = dst.getClass();
        Class srcClass = src.getClass();

        if (!dstClass.isArray() || !srcClass.isArray()) {
            try {
                java.lang.reflect.Method m = dstClass.getMethod("deep_copy_1234", dstClass);
                m.invoke(dst, src);
            } catch (Exception e) {
                throw new RuntimeException("Ошибка при вызове deep_copy_1234", e);
            }
            return;
        }

        int length = java.lang.reflect.Array.getLength(dst);
        if (length != java.lang.reflect.Array.getLength(src)) {
            throw new IllegalArgumentException("Массивы разной длины");
        }

        for (int i = 0; i < length; i++) {
            Object dstElem = java.lang.reflect.Array.get(dst, i);
            Object srcElem = java.lang.reflect.Array.get(src, i);

            Class compType = dstClass.getComponentType();
            if (compType.isPrimitive()) {
                java.lang.reflect.Array.set(dst, i, srcElem);
            } else {
                deepCopyArrays(dstElem, srcElem);
            }
        }
    }

}

// javac -source 1.5 -target 1.5 AdaUtility.java