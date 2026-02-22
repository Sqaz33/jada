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

    public static void initializeArray(Object array) {
        if (array == null) return;

        Class arrayClass = array.getClass();
        if (!arrayClass.isArray()) return; // не массив

        int length = java.lang.reflect.Array.getLength(array);
        Class compType = arrayClass.getComponentType();

        for (int i = 0; i < length; i++) {
            Object elem = java.lang.reflect.Array.get(array, i);

            if (compType.isArray()) {
                // Рекурсивно обходим существующий подмассив
                if (elem != null) {
                    initializeArray(elem);
                }
            } else if (compType.isPrimitive()) {
                // Примитивы JVM уже нули, можно явно
                if (compType == boolean.class) {
                    java.lang.reflect.Array.setBoolean(array, i, false);
                } else if (compType == byte.class) {
                    java.lang.reflect.Array.setByte(array, i, (byte) 0);
                } else if (compType == short.class) {
                    java.lang.reflect.Array.setShort(array, i, (short) 0);
                } else if (compType == int.class) {
                    java.lang.reflect.Array.setInt(array, i, 0);
                } else if (compType == long.class) {
                    java.lang.reflect.Array.setLong(array, i, 0L);
                } else if (compType == float.class) {
                    java.lang.reflect.Array.setFloat(array, i, 0f);
                } else if (compType == double.class) {
                    java.lang.reflect.Array.setDouble(array, i, 0d);
                } else if (compType == char.class) {
                    java.lang.reflect.Array.setChar(array, i, '\0');
                }
            } else {
                // Ссылочные типы — создаём объект через конструктор без аргументов
                if (elem == null) {
                    try {
                        Object obj = compType.newInstance(); // Java 5
                        java.lang.reflect.Array.set(array, i, obj);
                    } catch (Exception e) {
                        throw new RuntimeException("Не удалось создать объект " + compType.getName(), e);
                    }
                }
            }
        }
    }

}

// javac -source 1.5 -target 1.5 AdaUtility.java