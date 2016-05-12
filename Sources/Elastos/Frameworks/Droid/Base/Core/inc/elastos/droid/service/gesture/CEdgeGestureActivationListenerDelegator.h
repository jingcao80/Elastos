

        private class Delegator extends IEdgeGestureActivationListener.Stub {
            CARAPI constructor(
                /* [in] */ IEdgeGestureActivationListener* weakHost);

            public void onEdgeGestureActivation(final int touchX, final int touchY, final int positionIndex, final int flags)
                    throws RemoteException {
                mHandler.post(new Runnable() {
                    public void run() {
                        EdgeGestureActivationListener.this.onEdgeGestureActivation(touchX, touchY, EdgeGesturePosition.values()[positionIndex], flags);
                    }
                });
            }
        }