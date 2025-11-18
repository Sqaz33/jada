// File: OuterClass.java
public class OuterClass {
    private static String outerStaticField = "Outer Static";
    private String outerInstanceField = "Outer Instance";
    
    // Статический вложенный класс
    public static class StaticNestedClass {
        private String nestedField;
        
        public StaticNestedClass(String value) {
            this.nestedField = value;
            // Имеет доступ только к статическим членам внешнего класса
            System.out.println("From static nested: " + outerStaticField);
            // System.out.println(outerInstanceField); // Ошибка компиляции!
        }
        
        public void print() {
            System.out.println("StaticNested: " + nestedField);
        }
    }
    
    public static void main(String[] args) {
        // Static nested class можно создать без экземпляра OuterClass
        StaticNestedClass staticNested = new StaticNestedClass("Direct");
        staticNested.print();
    }
}