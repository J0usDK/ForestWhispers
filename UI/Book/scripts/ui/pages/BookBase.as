package ui.pages {
    import flash.display.MovieClip;
    import flash.events.Event;
    import flash.external.ExternalInterface;

    public class BookBase extends MovieClip {
        private var _isOpen:Boolean = false;
        private var _currentSpread:int = 0;
        private var _maxSpreads:int = 1;

        private var _leftPage:MovieClip;
        private var _rightPage:MovieClip;

        public var leftPageContainer_mc:MovieClip;
        public var rightPageContainer_mc:MovieClip;

        public var inventory_mc:InventoryPage;
        
        public function BookBase() {
            super();
            this.visible = false;
            inventory_mc = new InventoryPage();
        }

        public function setMaxSpreads(count:int):void {
            _maxSpreads = count;
        }

        public function openBook():void {
            if (_isOpen) return;

            this.visible = true;
            _isOpen = true;
            setSpread(0);
        }

        public function closeBook():void {
            if (!_isOpen) return;

            this.visible = false;
            _isOpen = false;
        }

        public function setSpread(spreadIndex:int):void {
            if (spreadIndex < 0 || spreadIndex >= _maxSpreads) return;

            _currentSpread = spreadIndex;
            hideAllPages();

            switch (_currentSpread) {
                case 0:
                    leftPageContainer_mc.addChild(inventory_mc);
                    inventory_mc.visible = true;
                    break;
            }
        }

        private function hideAllPages():void {
            inventory_mc.visible = false;
            
            if (inventory_mc.parent == leftPageContainer_mc || inventory_mc.parent == rightPageContainer_mc) {
                inventory_mc.parent.removeChild(inventory_mc);
            }
        }

        public function nextSpread():void {
            if (_currentSpread < _maxSpreads - 1) {
                setSpread(_currentSpread + 1);
            }
        }

        public function prevSpread():void {
            if (_currentSpread > 0) {
                setSpread(_currentSpread - 1);
            }
        }

        public function get currentSpread():int {
            return _currentSpread;
        }
    }
}