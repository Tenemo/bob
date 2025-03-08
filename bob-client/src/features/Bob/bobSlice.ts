import { createSlice } from '@reduxjs/toolkit';
import type { PayloadAction } from '@reduxjs/toolkit';

export type BobState = {
    ip: string;
    apiKey: string | null;
};

const initialState: BobState = {
    ip: '',
    apiKey: null,
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
         * Sets the OpenAI API key.
         * @param state - Current state.
         * @param action - Payload containing the API key.
         */
        setOpenaiApiKey: (state, action: PayloadAction<string>) => {
            state.apiKey = action.payload;
        },
    },
});

export const { setIp, setOpenaiApiKey } = bobSlice.actions;

export const selectBobIp = (state: { bob: BobState }): string => state.bob.ip;
export const selectApiKey = (state: { bob: BobState }): string | null =>
    state.bob.apiKey;
