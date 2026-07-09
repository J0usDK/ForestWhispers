package data {
    import flash.utils.Dictionary;

    public class Localization {
        private static var _dict:Dictionary = new Dictionary();
        private static var _initialized:Boolean = false;

        public static function setTable(...args):void {
            _dict = new Dictionary();

            for (var i:int = 0; i < args.length - 1; i += 2) {
                var key:uint = uint(args[i]);
                var value:String = String(args[i + 1]);
                _dict[key] = value;
            }
            _initialized = true;
        }

        public static function resolve(id:uint):String {
            if (!_initialized) return "<missing string>";

            var result:* = _dict[id];
            return (result != undefined) ? String(result) : "<missing string>";
        }

        public static function get isReady():Boolean {
            return _initialized;
        }
    }
}