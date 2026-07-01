package com.unipi.PlayerHive.DTO.analytics;

import lombok.AllArgsConstructor;
import lombok.Getter;

@AllArgsConstructor
@Getter
public class GenreStatsDTO {
    private String genre;
    private Double avgScore;
    private Double avgHoursPerPlayer;
    private Integer totalGames;

}
