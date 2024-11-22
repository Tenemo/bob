import type { Action, ThunkAction } from '@reduxjs/toolkit';
import { configureStore } from '@reduxjs/toolkit';
import { combineReducers } from 'redux';
import { createLogger } from 'redux-logger';
import {
    persistStore,
    persistReducer,
    FLUSH,
    REHYDRATE,
    PAUSE,
    PERSIST,
    PURGE,
    REGISTER,
} from 'redux-persist';
import hardSet from 'redux-persist/lib/stateReconciler/hardSet';
import storage from 'redux-persist/lib/storage';

import { bobSlice } from 'features/Bob/bobSlice';
import { bobApi } from 'features/BobApi/bobApi';

export type RootState = ReturnType<typeof rootReducer>;

const IS_LOGGING_ENABLED = false;

const persistConfig = {
    key: 'root',
    storage,
    stateReconciler: hardSet,
    blacklist: [bobApi.reducerPath],
    version: 1,
};

const logger = createLogger({
    diff: true,
    collapsed: true,
});

export const rootReducer = combineReducers({
    bob: bobSlice.reducer,
    bobApi: bobApi.reducer,
});

const persistedReducer = persistReducer<RootState>(persistConfig, rootReducer);

const middleware = [bobApi.middleware];

export const store = configureStore({
    reducer: persistedReducer,
    devTools: true, // Leaving it on for prod on purpose
    middleware: (getDefaultMiddleware) => {
        const defaultMiddleware = getDefaultMiddleware({
            serializableCheck: {
                ignoredActions: [
                    FLUSH,
                    REHYDRATE,
                    PAUSE,
                    PERSIST,
                    PURGE,
                    REGISTER,
                ],
                ignoredPaths: [bobApi.reducerPath],
            },
        });

        // eslint-disable-next-line @typescript-eslint/no-unnecessary-condition
        return IS_LOGGING_ENABLED
            ? defaultMiddleware.concat(middleware).concat(logger)
            : defaultMiddleware.concat(middleware);
    },
});
export const persistor = persistStore(store);

export type AppStore = typeof store;
export type AppDispatch = AppStore['dispatch'];
export type AppThunk<ThunkReturnType = void> = ThunkAction<
    ThunkReturnType,
    RootState,
    unknown,
    Action
>;
