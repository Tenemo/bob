import { createSlice } from '@reduxjs/toolkit';
import type { PayloadAction } from '@reduxjs/toolkit';

export type BobState = {
    ip: string;
    isConnected: boolean;
};

const initialState: BobState = {
    ip: '',
    isConnected: false,
};

export const bobSlice = createSlice({
    name: 'bob',
    initialState,
    reducers: {
        /**
         * Sets Bob's IP address.
         * @param state - Current state.
         * @param action - Payload containing the IP address.
         */
        setIp: (state, action: PayloadAction<string>) => {
            state.ip = action.payload;
        },
        /**
         * Sets Bob's connection status.
         * @param state - Current state.
         * @param action - Payload containing the connection status.
         */
        setConnectionStatus: (state, action: PayloadAction<boolean>) => {
            state.isConnected = action.payload;
        },
    },
});

export const { setIp, setConnectionStatus } = bobSlice.actions;

export const selectBobIp = (state: { bob: BobState }): string => state.bob.ip;
export const selectBobConnectionStatus = (state: { bob: BobState }): boolean =>
    state.bob.isConnected;
