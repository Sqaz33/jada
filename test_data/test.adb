with Ada.Text_IO;
use Ada.Text_IO;

procedure Main is
   --  X: Integer;

   --  arr: array(1..3) of Integer;

   --  procedure proc(V: Integer) is
   --     x: Integer;
   --     Y: Integer := 1;
   --  begin
   --     X := Y;
   --  end proc;

   --  function proc(V: Integer) return Integer is -- no err with this
   --  begin
   --     return 1;
   --  end proc;


   --  type TyN is record
   --     Z: Integer;
   --  end record;

   --  type Ty is record
   --     Y: TyN;
   --  end record;

   --  package pack is
   --     package pack is 
   --        L2: Ty;
   --     end pack;
   --  end pack;

   --  package pack is
   --     type cls is tagged record
   --        X: Integer := 1;
   --     end record;

   --     procedure proc(self: in out cls);

   --     type cls2 is new cls with record
   --        Y: Integer;
   --     end record;

   --     type cls3 is new cls2 with record
   --        Z: Integer;
   --     end record;

   --     type cls4 is tagged record    
   --        M: Integer;
   --     end record;
   --  end pack;

   --  package body pack is
   --     procedure proc(self: in out cls) is
   --     begin
   --        self.X := 1;
   --     end proc;
   --  end pack;

   --  obj: pack.cls;
   --  obj2: pack.cls2;
   --  obj3: pack.cls3;
   --  obj67: pack.cls4;

   --  procedure proc2(f: in out pack.cls'Class) is
   --  begin
   --     f.x := 2;
   --  end proc2;

   --  type Ty1 is record 
   --     X: Integer;
   --  end record;

   --  function func return Ty1 is
   --     X: Ty1;
   --  begin
   --     return X;
   --  end func;

   --  procedure stringP(str: out String(1..11)) is 
   --  begin 
   --     str(1) := 'c';
   --  end stringP;

   --  str: String(1..10);

begin 
   --  X := Integer'Image(1);
   -- 1;
   --  X := proc(proc(1));
   -- proc(proc(1)); -- err
   -- arr(1) := 1;
   --  X := pack.pack.L2.Y.Z;
   --  obj.proc;
   --  obj.X := 1;
   --  obj2.Y := 1;
   
   --  obj3.proc; 
   --  proc2(obj); 
   --  X := obj.X + 1 + obj2.X + 2 + 4 + 5 + obj67.M + proc(1) + 1;
   --  X := proc(1);
   
   --  stringP(str);
   --  1 + 1 := 1;

   --  func.X := 1;
   f(1);
end Main;
