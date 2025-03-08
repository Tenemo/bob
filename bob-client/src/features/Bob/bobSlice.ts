import { createSlice } from '@reduxjs/toolkit';
import type { PayloadAction } from '@reduxjs/toolkit';

export type BobState = {
    ip: string;
};

const initialState: BobState = {
    ip: '',
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
    },
});

export const { setIp } = bobSlice.actions;

export const selectBobIp = (state: { bob: BobState }): string => state.bob.ip;
