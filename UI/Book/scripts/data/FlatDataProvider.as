package data {
    import flash.events.Event;
    import flash.events.EventDispatcher;
    import scaleform.clik.interfaces.IDataProvider;
    import data.Localization;

    public dynamic class FlatDataProvider extends EventDispatcher implements IDataProvider {
        private var _flatData:Array;
        private var _stride:int;

        public function FlatDataProvider(data:Array = null, stride:int = 2) {
            super();
            _flatData = data || [];
            _stride = stride;
        }

        public function set rawData(value:Array):void {
            _flatData = value || [];
            invalidate();
        }

        public function get rawData():Array {
            return _flatData;
        }

        public function pushItem(...args):void {
            if (args.length != _stride) {
                throw new Error("Number of arguments must match the stride.");
            }

            _flatData.push.apply(null, args);
            invalidate();
        }

        public function removeItemAt(index:int):void {
            var dataIndex:int = index * _stride;

            if (dataIndex >= 0 && dataIndex < _flatData.length) {
                _flatData.splice(dataIndex, _stride);
                invalidate();
            }
        }

        public function clear():void {
            _flatData = [];
            invalidate();
        }

        public function get length():uint {
            return _flatData.length / _stride;
        }

        public function requestItemAt(index:uint, callBack:Function = null):Object {
            var item:Object = null;
            var dataIndex:int = index * _stride;

            if (dataIndex >= 0 && dataIndex < _flatData.length) {
                item = {
                    name: Localization.resolve(uint(_flatData[dataIndex])),
                    weight: _flatData[dataIndex + 1].toFixed(2),
                    type: _flatData[dataIndex + 2]
                };
            }

            if (callBack != null) {
                callBack(item);
            }

            return item;
        }

        public function requestItemRange(startIndex:int, endIndex:int, callBack:Function = null):Array {
            var items:Array = [];

            for (var i:int = startIndex; i <= endIndex; i++) {
                var item:Object = requestItemAt(i);
                if (item != null) items.push(item);
            }

            if (callBack != null) {
                callBack(items);
            }

            return items;
        }

        public function indexOf(item:Object, callBack:Function=null):int {
            return -1;
        }

        public function cleanUp():void {
            _flatData = [];
        }

        public function invalidate(length:uint=0):void {
            dispatchEvent(new Event(Event.CHANGE));
        }
    }
}