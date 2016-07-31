package
{
	import flash.external.ExtensionContext;
	
	public class RawKeyboard
	{
		 
		private var mExtensionContext:ExtensionContext;
		
		public var mKeyboardID:uint;
		
		public function RawKeyboard(param1:uint, param2:ExtensionContext)
		{
			super();
			mKeyboardID = param1;
			mExtensionContext = param2;
		}
		
		public function GetDownState(param1:Boolean) : uint {
			return mExtensionContext.call("MultiKeyboard_GetDownState",mKeyboardID,param1) as uint;
		}
		
		public function GetActionThisFrame() : uint {
			return mExtensionContext.call("MultiKeyboard_GetActionThisFrame",mKeyboardID) as uint;
		}
		
		public function SetInputKey(param1:Boolean, param2:uint, param3:uint) : void {
			mExtensionContext.call("MultiKeyboard_SetInputMapping",mKeyboardID,param3,param2,param1);
		}
		
		public function ClearInputMappings(param1:Boolean) : void {
			mExtensionContext.call("MultiKeyboard_ClearInputMappings",mKeyboardID,param1);
		}
		
		public function GetRawKeyboardName() : String {
			return mExtensionContext.call("MultiKeyboard_GetRawKeyboardName",mKeyboardID) as String;
		}
	}
}
