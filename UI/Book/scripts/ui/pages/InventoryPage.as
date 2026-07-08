package ui.pages {
    import flash.display.MovieClip;
    import flash.events.Event;
    import flash.events.MouseEvent;
    import flash.external.ExternalInterface;
    import scaleform.clik.controls.ScrollingList;
    import data.FlatDataProvider;
    import enums.PageType;

    public class InventoryPage extends MovieClip {
        public var inventoryList:ScrollingList;

        public var sortType_btn:MovieClip;
        public var sortName_btn:MovieClip;
        public var sortWeight_btn:MovieClip;

        private var _provider:FlatDataProvider;

        public function InventoryPage() {
            super();
            _provider = new FlatDataProvider([], 3);
            this.addEventListener(Event.ADDED_TO_STAGE, onInit);
        }

        private function onInit(e:Event):void {
            this.removeEventListener(Event.ADDED_TO_STAGE, onInit);
            this.addEventListener(Event.REMOVED_FROM_STAGE, onRemoved);

            inventoryList.dataProvider = _provider;
            setupButtons();
        }

        private function onRemoved(e:Event):void {
            this.removeEventListener(Event.REMOVED_FROM_STAGE, onRemoved);

            var root = this.root as Object;
            delete root["Inv_SetItems"];
            delete root["Inv_ClearInventory"];
            delete root["Inv_AddItem"];
            delete root["Inv_RemoveItemAt"];

            if (ExternalInterface.available) {
                ExternalInterface.call("UI_PageRemoved", PageType.INVENTORY);
            }
        }

        private function setupButtons():void {
            sortType_btn.buttonMode = true;
            sortName_btn.buttonMode = true;
            sortWeight_btn.buttonMode = true;
            
            sortType_btn.addEventListener(MouseEvent.CLICK, onSortType);
            sortName_btn.addEventListener(MouseEvent.CLICK, onSortName);
            sortWeight_btn.addEventListener(MouseEvent.CLICK, onSortWeight);
        }

        private function onSortType(e:MouseEvent):void {
            if (ExternalInterface.available) {
                ExternalInterface.call("UI_OnSortType");
            }
        }

        private function onSortName(e:MouseEvent):void {
            if (ExternalInterface.available) {
                ExternalInterface.call("UI_OnSortName");
            }
        }

        private function onSortWeight(e:MouseEvent):void {
            if (ExternalInterface.available) {
                ExternalInterface.call("UI_OnSortWeight");
            }
        }

        public function setItems(...args):void {
            if (args == null || args.length < 3) return;
            _provider.rawData = args;
        }

        public function addItem(nameStringID:uint, weight:Number, type:int):void {
            _provider.pushItem(nameStringID, weight, type);
        }

        public function removeItemAt(index:int):void {
            _provider.removeItemAt(index);
        }

        public function clearInventory():void {
            _provider.clear();
        }
    }
}