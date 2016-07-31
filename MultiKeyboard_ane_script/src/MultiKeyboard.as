package 
{
	import flash.events.EventDispatcher;
	import flash.external.ExtensionContext;
	
	public class MultiKeyboard extends EventDispatcher
	{
		
		private static var sExtensionContext:ExtensionContext;
		
		private static var sbDisposed:Boolean;
		 
		public function MultiKeyboard()
		{
			super();
		}
		
		public static function Init() : Boolean {
			sExtensionContext = ExtensionContext.createExtensionContext("MultiKeyboard",null);
			if(!sExtensionContext) return false;
			sbDisposed = false;
			return sExtensionContext.call("MultiKeyboard_Init") as Boolean;
		}
		
		public static function ActivateRawKeyboard() : RawKeyboard {
			var res:uint = sExtensionContext.call("MultiKeyboard_ActivateRawKeyboard") as uint;
			if(res) {
				return new RawKeyboard(res,sExtensionContext);
			}
			return null;
		}
		
		public static function TickMessages() : uint {
			if(sbDisposed) {
				return 0;
			}
			return sExtensionContext.call("MultiKeyboard_TickMessages") as uint;
		}
		
		public static function Dispose() : void {
			if(!sbDisposed) {
				sExtensionContext.dispose();
				sbDisposed = true;
			}
		}
		
		public static function IsContextDisposed() : Boolean {
			return sbDisposed;
		}
	}
}
