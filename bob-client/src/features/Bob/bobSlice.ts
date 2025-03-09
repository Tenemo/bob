import { createSlice } from '@reduxjs/toolkit';
import type { PayloadAction } from '@reduxjs/toolkit';

export type BobState = {
    isDebug: boolean;
    useBobSpeaker: boolean;
};

const initialState: BobState = {
    isDebug: false,
    useBobSpeaker: true,
};

export const bobSlice = createSlice({
    name: 'bob',
    initialState,
    reducers: {
        /**
         * Sets debug mode.
         * @param state - Current state.
         * @param action - Payload containing the debug mode value.
         */
        setDebugMode: (state, action: PayloadAction<boolean>) => {
            state.isDebug = action.payload;
        },
        /**
         * Sets speaker mode (Bob's speaker vs device speaker).
         * @param state - Current state.
         * @param action - Payload containing the speaker mode value.
         */
        setUseBobSpeaker: (state, action: PayloadAction<boolean>) => {
            state.useBobSpeaker = action.payload;
        },
    },
});

export const { setDebugMode, setUseBobSpeaker } = bobSlice.actions;

export const selectIsDebug = (state: { bob: BobState }): boolean =>
    state.bob.isDebug;
export const selectUseBobSpeaker = (state: { bob: BobState }): boolean =>
    state.bob.useBobSpeaker;
