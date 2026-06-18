package com.unipi.PlayerHive.DTO.games;

import lombok.AllArgsConstructor;
import lombok.Getter;

@AllArgsConstructor
@Getter
public class TrendingGameDTO {
    private String name;
    private Integer socialPlayCount;
}
