import java.lang.reflect.*;
import java.util.IdentityHashMap;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;
import java.util.concurrent.atomic.AtomicReference; 
import java.util.Scanner;


public class AdaUtility {
    private static final Scanner SCANNER = new Scanner(System.in);

    public static void readBool(AtomicBoolean target) {
        Scanner sc = new Scanner(System.in);
        if (!sc.hasNextBoolean()) {
            throw new IllegalArgumentException("Expected boolean value");
        }
        boolean value = sc.nextBoolean();
        target.set(value);
    }

    public static void readInt(AtomicInteger target) {
        Scanner sc = new Scanner(System.in);
        if (!sc.hasNextInt()) {
            throw new IllegalArgumentException("Expected integer value");
        }
        int value = sc.nextInt();
        target.set(value);
    }

    public static void readChar(AtomicInteger target) {
        Scanner sc = new Scanner(System.in);
        String input = sc.nextLine();
        if (input.length() > 0) {
            char value = input.charAt(0);  // берем первый символ
            target.set((int) value);        // преобразуем char в int (ASCII код)
        }
    }

    public static void readFloat(AtomicReference<Float> target) {
        Scanner sc = new Scanner(System.in);
        if (!sc.hasNextFloat()) {
            throw new IllegalArgumentException("Expected float value");
        }
        float value = sc.nextFloat();
        target.set(value);
    }

    public static void readString(StringBuilder target) {
        String line = SCANNER.nextLine();
        target.setLength(0);
        target.append(line);
    }

    public static void printStringBuilder(StringBuilder sb) {
       if (sb == null) return;
        System.out.println(sb.toString());
    }

    public static void initArrayElements(Object array) {
        if (array == null)
            return;

        Class<?> clazz = array.getClass();
        if (!clazz.isArray())
            throw new IllegalArgumentException("Not an array");

        initRecursive(array);
    }

    private static void initRecursive(Object array) {
        Class<?> componentType = array.getClass().getComponentType();
        int length = Array.getLength(array);

        // Если это не массив массивов — мы на последнем измерении
        if (!componentType.isArray()) {

            // Примитивы не трогаем
            if (componentType.isPrimitive())
                return;

            for (int i = 0; i < length; i++) {
                try {
                    Object instance = componentType.newInstance();
                    Array.set(array, i, instance);
                } catch (Exception e) {
                    throw new RuntimeException(e);
                }
            }
            return;
        }

        // Иначе идём глубже
        for (int i = 0; i < length; i++) {
            Object subArray = Array.get(array, i);
            if (subArray != null)
                initRecursive(subArray);
        }
    }

    // concat
    public static StringBuilder concat(StringBuilder sb1, StringBuilder sb2) {
        if (sb1 == null) sb1 = new StringBuilder();
        if (sb2 == null) sb2 = new StringBuilder();

        StringBuilder result = new StringBuilder(sb1.length() + sb2.length());
        result.append(sb1);
        result.append(sb2);
        return result;
    }

    //string idx
    public static void setCharAt(StringBuilder sb, int index, char value) {
        if (sb == null)
            throw new NullPointerException("StringBuilder is null");

        if (index < 0 || index >= sb.length())
            throw new IndexOutOfBoundsException("Index: " + index);

        sb.setCharAt(index, value);
    }

    public static char charAt(StringBuilder sb, int index) {
        if (sb == null)
            throw new NullPointerException("StringBuilder is null");

        if (index < 0 || index >= sb.length())
            throw new IndexOutOfBoundsException("Index: " + index);

        return sb.charAt(index);
    }

    public static StringBuilder fromStringLiteral(String value) {
        if (value == null)
            return null;

        return new StringBuilder(value);
    }

    // image
    public static StringBuilder imageFromChar(char value) {
        StringBuilder sb = new StringBuilder(1);
        sb.append(value);
        return sb;
    }

    public static StringBuilder imageFromInt(int value) {
        StringBuilder sb = new StringBuilder();
        sb.append(value);
        return sb;
    }

    public static StringBuilder imageFromBool(boolean value) {
        StringBuilder sb = new StringBuilder();
        sb.append(value);
        return sb;
    }

    public static StringBuilder imageFromFloat(float value) {
        StringBuilder sb = new StringBuilder();
        sb.append(value);
        return sb;
    }

    // -----------------------------
    // 1) Создать "атомик" из примитива
    public static AtomicBoolean toAtomic(boolean value) {
        return new AtomicBoolean(value);
    }

    public static AtomicInteger toAtomic(int value) {
        return new AtomicInteger(value);
    }

    public static AtomicReference<Float> toAtomic(float value) {
        // нет AtomicFloat в Java5 → используем AtomicReference<Float>
        return new AtomicReference<Float>(Float.valueOf(value));
    }

    public static AtomicInteger toAtomic(char value) {
        // нет AtomicChar → используем AtomicInteger с кодом символа
        return new AtomicInteger((int) value);
    }

    // -----------------------------
    // 2) Извлечь значение из атомика
    public static boolean fromAtomic(AtomicBoolean atomic) {
        return atomic.get();
    }

    public static int fromAtomic(AtomicInteger atomic) {
        return atomic.get();
    }

    public static float fromAtomic(AtomicReference<Float> atomic) {
        return atomic.get().floatValue();
    }

    public static char fromAtomicChar(AtomicInteger atomic) {
        return (char) atomic.get();
    }

    // -----------------------------
    // Установка значения в атомик
    public static void setAtomic(AtomicBoolean atomic, boolean value) {
        atomic.set(value);
    }

    public static void setAtomic(AtomicInteger atomic, int value) {
        atomic.set(value);
    }

    public static void setAtomic(AtomicReference<Float> atomic, float value) {
        atomic.set(Float.valueOf(value));
    }

    public static void setAtomicChar(AtomicInteger atomic, char value) {
        atomic.set((int) value);
    }

    // -----------------------------
    // штуки
    public static StringBuilder copyStringBuilder(StringBuilder src) {
        if (src == null)
            return null;

        StringBuilder copy = new StringBuilder(src.length());
        copy.append(src);
        return copy;
    }

    public static Object deepCopy(Object src) {
        if (src == null)
            return null;

        Context ctx = new Context();
        return ctx.copyAny(src);
    }

    public static Object deepCopyArray(Object srcArray) {
        if (srcArray == null)
            return null;

        Context ctx = new Context();
        return ctx.copyArray(srcArray);
    }

    private static class Context {

        private IdentityHashMap<Object, Object> visited =
                new IdentityHashMap<Object, Object>();

        private Object copyAny(Object src) {

            if (visited.containsKey(src))
                return visited.get(src);

            Class type = src.getClass();

            if (type.isArray())
                return copyArray(src);

            if (type.isPrimitive()
                    || src instanceof String
                    || src instanceof Number
                    || src instanceof Boolean
                    || src instanceof Character) {
                return src;
            }

            try {
                Object newObj = type.newInstance();
                visited.put(src, newObj);
                copyFields(newObj, src);
                return newObj;
            } catch (Exception e) {
                throw new RuntimeException(e);
            }
        }

        private void copyFields(Object dst, Object src)
                throws IllegalAccessException, InstantiationException {

            Class clazz = src.getClass();

            while (clazz != null) {

                Field[] fields = clazz.getDeclaredFields();

                for (int i = 0; i < fields.length; i++) {

                    Field f = fields[i];

                    if (Modifier.isStatic(f.getModifiers()))
                        continue;

                    f.setAccessible(true);

                    Object value = f.get(src);

                    if (value == null) {
                        f.set(dst, null);
                    }
                    else if (f.getType().isPrimitive()) {
                        f.set(dst, value);
                    }
                    else {
                        Object copied = copyAny(value);
                        f.set(dst, copied);
                    }
                }

                clazz = clazz.getSuperclass();
            }
        }

        private Object copyArray(Object array) {

            if (visited.containsKey(array))
                return visited.get(array);

            int length = Array.getLength(array);
            Class compType =
                    array.getClass().getComponentType();

            Object newArray =
                    Array.newInstance(compType, length);

            visited.put(array, newArray);

            for (int i = 0; i < length; i++) {

                Object value = Array.get(array, i);

                if (value == null) {
                    Array.set(newArray, i, null);
                }
                else if (compType.isPrimitive()) {
                    Array.set(newArray, i, value);
                }
                else {
                    Object copied = copyAny(value);
                    Array.set(newArray, i, copied);
                }
            }

            return newArray;
        }
    }

    // javac -source 1.5 -target 1.5 AdaUtility.java 
}